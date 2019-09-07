#include "dfontpreviewlistview.h"
#include "dfontinfomanager.h"
#include "dfontpreviewitemdelegate.h"
#include "globaldef.h"
#include "dfmxmlwrapper.h"

#include <DLog>
#include <DMenu>

#include <QFontDatabase>
#include <QSet>

DWIDGET_USE_NAMESPACE

DFontPreviewListView::DFontPreviewListView(QWidget *parent)
    : DListView(parent)
    , m_dbManager(DFMDBManager::instance())
{
    setAutoScroll(false);
    setMouseTracking(true);

    initFontListData();
    initDelegate();
    initConnections();
}

DFontPreviewListView::~DFontPreviewListView()
{
}

void DFontPreviewListView::initFontListData()
{
    QStringList fontNameList;
    DFontInfoManager *fontInfoMgr = DFontInfoManager::instance();
    QStringList disableFontList = DFMXmlWrapper::getFontConfigDisableFontPathList();

    m_fontPreviewItemModel = new QStandardItemModel;

    int recordCount = m_dbManager->getRecordCount();
    if (recordCount > 0) {

        //从fontconfig配置文件同步字体启用/禁用状态数据
        syncFontEnableDisableStatusData(disableFontList);

        refreshFontListData(m_fontPreviewItemModel, true);

        return;
    }

    //开启事务
    m_dbManager->beginTransaction();

    QStringList strAllFontList = fontInfoMgr->getAllFontPath();
    qDebug() << "strAllFontList.size()" << strAllFontList.size() << endl;
    for (int i = 0; i < strAllFontList.size(); ++i) {
        QString filePath = strAllFontList.at(i);
        if (filePath.length() > 0) {
            insertFontItemData(m_fontPreviewItemModel, filePath, i + 1);
        }
    }

    m_dbManager->endTransaction();
}

void DFontPreviewListView::insertFontItemData(QStandardItemModel *sourceModel, QString filePath, int index)
{
    DFontInfoManager *fontInfoMgr = DFontInfoManager::instance();
    DFontPreviewItemData itemData;
    QFileInfo filePathInfo(filePath);
    itemData.pFontInfo = fontInfoMgr->getFontInfo(filePath);

    if (itemData.pFontInfo->styleName.length() > 0) {
        itemData.strFontName =
            QString("%1-%2").arg(itemData.pFontInfo->familyName).arg(itemData.pFontInfo->styleName);
    } else {
        itemData.strFontName = itemData.pFontInfo->familyName;
    }

    itemData.strFontId = QString::number(index);
    itemData.appFontId = index;
    itemData.strFontFileName = filePathInfo.baseName();
    itemData.strFontPreview = FTM_DEFAULT_PREVIEW_TEXT;
    itemData.iFontSize = FTM_DEFAULT_PREVIEW_FONTSIZE;
    itemData.isEnabled = true;
    itemData.isCollected = false;
    itemData.pFontInfo->isInstalled = true;

    QStandardItem *item = new QStandardItem;
    item->setData(QVariant::fromValue(itemData), Qt::DisplayRole);

    sourceModel->appendRow(item);

    m_dbManager->addFontInfo(itemData);
}

void DFontPreviewListView::refreshFontListData(QStandardItemModel *sourceModel, bool isStartup)
{
    DFontInfoManager *fontInfoMgr = DFontInfoManager::instance();
    QStringList strAllFontList = fontInfoMgr->getAllFontPath();

    QList<DFontPreviewItemData> fontInfoList = m_dbManager->getAllFontInfo();

    //开启事务
    m_dbManager->beginTransaction();

    QSet<QString> dbFilePathSet;
    for (int i = 0; i < fontInfoList.size(); ++i) {

        DFontPreviewItemData itemData = fontInfoList.at(i);
        QString filePath = itemData.pFontInfo->filePath;
        QFileInfo filePathInfo(filePath);
        //如果字体文件已经不存在，则从t_manager表中删除
        if (!filePathInfo.exists()) {
            m_dbManager->deleteFontInfo("fontId", itemData.strFontId);
            continue;
        }

        QStandardItem *item = new QStandardItem;
        item->setData(QVariant::fromValue(itemData), Qt::DisplayRole);

        if (isStartup) {
            sourceModel->appendRow(item);
        }

        dbFilePathSet.insert(filePath);
    }

    //根据文件路径比较出不同的字体文件
    QSet<QString> fontListSet = strAllFontList.toSet();
    QSet<QString> diffSet = fontListSet.subtract(dbFilePathSet);
    qDebug() << "diffSet count:" << diffSet.count();
    if (diffSet.count() > 0) {
        int maxFontId = m_dbManager->getCurrMaxFontId();
        QList<QString> diffFilePathList = diffSet.toList();
        for (int i = 0; i < diffFilePathList.size(); ++i) {
            QString filePath = diffFilePathList.at(i);
            if (filePath.length() > 0) {
                insertFontItemData(sourceModel, filePath, maxFontId + i + 1);
            }
        }
    }

    m_dbManager->endTransaction();
}

void DFontPreviewListView::syncFontEnableDisableStatusData(QStringList disableFontPathList)
{
    //disableFontPathList为被禁用的字体路径列表
    if (disableFontPathList.size() == 0) {
        return;
    }

    QMap<QString, bool> disableFontMap;
    for(int i=0; i<disableFontPathList.size(); i++) {
        QString disableFontPath = disableFontPathList.at(i);
        disableFontMap.insert(disableFontPath, true);
    }

    //开启事务
    m_dbManager->beginTransaction();

    QList<DFontPreviewItemData> fontInfoList = m_dbManager->getAllFontInfo();

    for(int i=0; i<fontInfoList.size(); i++) {
        DFontPreviewItemData fontItemData = fontInfoList.at(i);
        QString keyFilePath = fontItemData.pFontInfo->filePath;

        //disableFontMap为被禁用的字体map
        if (disableFontMap.value(keyFilePath)) {
            fontItemData.isEnabled = false;
        }
        else {
            fontItemData.isEnabled = true;
        }

        m_dbManager->updateFontInfoByFontFilePath(keyFilePath, "isEnabled", QString::number(fontItemData.isEnabled));
    }

    m_dbManager->endTransaction();
}

void DFontPreviewListView::initDelegate()
{
    m_fontPreviewItemDelegate = new DFontPreviewItemDelegate(this);
    this->setItemDelegate(m_fontPreviewItemDelegate);

    m_fontPreviewProxyModel = new DFontPreviewProxyModel(this);
    m_fontPreviewProxyModel->setSourceModel(m_fontPreviewItemModel);
    m_fontPreviewProxyModel->setDynamicSortFilter(true);
    this->setModel(m_fontPreviewProxyModel);
}

void DFontPreviewListView::initConnections()
{
    connect(this, &DFontPreviewListView::onClickEnableButton, this,
            &DFontPreviewListView::onListViewItemEnableBtnClicked);
    connect(this, &DFontPreviewListView::onClickCollectionButton, this,
            &DFontPreviewListView::onListViewItemCollectionBtnClicked);
    connect(this, &DFontPreviewListView::onShowContextMenu, this,
            &DFontPreviewListView::onListViewShowContextMenu);
}

void DFontPreviewListView::mouseMoveEvent(QMouseEvent *event)
{
    DListView::mouseMoveEvent(event);

    QPoint mousePos = event->pos();

    QModelIndex rowModelIndex = indexAt(event->pos());
    QRect rect = rectForIndex(rowModelIndex);

    int collectIconSize = 22;
    QRect collectIconRect =
        QRect(rect.right() - 33, rect.top() + 10, collectIconSize, collectIconSize);

    DFontPreviewItemData itemData =
        qvariant_cast<DFontPreviewItemData>(m_fontPreviewProxyModel->data(rowModelIndex));

    if (collectIconRect.contains(mousePos)) {
        itemData.collectIconStatus = IconHover;
    }
    else {
        itemData.collectIconStatus = IconNormal;
    }
    m_fontPreviewProxyModel->setData(rowModelIndex, QVariant::fromValue(itemData), Qt::DisplayRole);
}

void DFontPreviewListView::mousePressEvent(QMouseEvent *event)
{
    DListView::mousePressEvent(event);

    QPoint mousePos = event->pos();

    QModelIndex rowModelIndex = indexAt(event->pos());
    QRect rect = rectForIndex(rowModelIndex);

    int collectIconSize = 22;
    QRect collectIconRect =
        QRect(rect.right() - 33, rect.top() + 10, collectIconSize, collectIconSize);

    DFontPreviewItemData itemData =
        qvariant_cast<DFontPreviewItemData>(m_fontPreviewProxyModel->data(rowModelIndex));

    if (collectIconRect.contains(mousePos)) {
        itemData.collectIconStatus = IconPress;
    }
    else {
        itemData.collectIconStatus = IconNormal;
    }
    m_fontPreviewProxyModel->setData(rowModelIndex, QVariant::fromValue(itemData), Qt::DisplayRole);

    if (event->button() == Qt::LeftButton) {
        m_bLeftMouse = true;
    } else {
        m_bLeftMouse = false;
    }
}

void DFontPreviewListView::mouseReleaseEvent(QMouseEvent *event)
{
    DListView::mouseReleaseEvent(event);

    QPoint selectionPoint = event->pos();

    QModelIndex rowModelIndex = indexAt(selectionPoint);
    m_currModelIndex = rowModelIndex;

    DFontPreviewItemData itemData =
        qvariant_cast<DFontPreviewItemData>(m_fontPreviewProxyModel->data(rowModelIndex));

    itemData.collectIconStatus = IconNormal;
    m_fontPreviewProxyModel->setData(rowModelIndex, QVariant::fromValue(itemData), Qt::DisplayRole);

    if (selectionPoint.x() < 50) {
        //触发启用/禁用字体
        emit onClickEnableButton(rowModelIndex);
    } else if ((selectionPoint.x() > (this->width() - 50)) &&
               (selectionPoint.x() < this->width())) {
        //触发收藏/取消收藏
        emit onClickCollectionButton(rowModelIndex);
    }
}

void DFontPreviewListView::setSelection(const QRect &rect,
                                        QItemSelectionModel::SelectionFlags command)
{
    DListView::setSelection(rect, command);

    QPoint selectionPoint(rect.x(), rect.y());
    QModelIndex modelIndex = indexAt(selectionPoint);
    qDebug() << modelIndex.row() << endl;
    m_currModelIndex = modelIndex;

    if (!m_bLeftMouse) {
        emit onShowContextMenu(modelIndex);
    }
}

void DFontPreviewListView::setModel(QAbstractItemModel *model)
{
    m_fontPreviewItemModel = qobject_cast<QStandardItemModel *>(model);
    DListView::setModel(model);
}

bool DFontPreviewListView::enableFont(DFontPreviewItemData itemData)
{
    QString fontConfigPath = DFMXmlWrapper::m_fontConfigFilePath;
    bool isCreateSuccess = DFMXmlWrapper::createFontConfigFile(fontConfigPath);

    if (!isCreateSuccess) {
        return false;
    }

    QStringList strFontPathList;
    DFMXmlWrapper::queryAllChildNodes_Text(fontConfigPath, "rejectfont", strFontPathList);

    QString fontFilePath = itemData.pFontInfo->filePath;
    qDebug() << strFontPathList << endl;
    qDebug() << fontFilePath << endl;
    if (strFontPathList.contains(fontFilePath)) {
        return DFMXmlWrapper::deleteNodeWithText(fontConfigPath, "pattern", fontFilePath);
    }

    return false;
}

bool DFontPreviewListView::disableFont(DFontPreviewItemData itemData)
{
    QString fontConfigPath = DFMXmlWrapper::m_fontConfigFilePath;
    bool isCreateSuccess = DFMXmlWrapper::createFontConfigFile(fontConfigPath);

    if (!isCreateSuccess) {
        return false;
    }

    QStringList strDisableFontPathList = DFMXmlWrapper::getFontConfigDisableFontPathList();

    QString fontFilePath = itemData.pFontInfo->filePath;
    qDebug() << strDisableFontPathList << endl;
    if (!strDisableFontPathList.contains(fontFilePath)) {
        return DFMXmlWrapper::addPatternNodesWithText(fontConfigPath, "rejectfont", fontFilePath);
    }

    return false;
}

void DFontPreviewListView::onListViewItemEnableBtnClicked(QModelIndex index)
{
    DFontPreviewItemData itemData =
        qvariant_cast<DFontPreviewItemData>(m_fontPreviewProxyModel->data(index));
    itemData.isEnabled = !itemData.isEnabled;

    qDebug()<< "familyName" <<itemData.pFontInfo->familyName << endl;

    if (itemData.isEnabled) {
        enableFont(itemData);
    } else {
        disableFont(itemData);
    }

    m_dbManager->updateFontInfoByFontId(itemData.strFontId, "isEnabled", QString::number(itemData.isEnabled));

    m_fontPreviewProxyModel->setData(index, QVariant::fromValue(itemData), Qt::DisplayRole);
}

void DFontPreviewListView::onListViewItemCollectionBtnClicked(QModelIndex index)
{
    DFontPreviewItemData itemData =
        qvariant_cast<DFontPreviewItemData>(m_fontPreviewProxyModel->data(index));
    itemData.isCollected = !itemData.isCollected;

    m_dbManager->updateFontInfoByFontId(itemData.strFontId, "isCollected", QString::number(itemData.isCollected));

    m_fontPreviewProxyModel->setData(index, QVariant::fromValue(itemData), Qt::DisplayRole);
}

void DFontPreviewListView::onListViewShowContextMenu(QModelIndex index)
{
    Q_UNUSED(index)

    DMenu *rightMenu = m_rightMenu;

    //在当前鼠标位置显示
    rightMenu->exec(QCursor::pos());
}

void DFontPreviewListView::setRightContextMenu(QMenu *rightMenu)
{
    m_rightMenu = rightMenu;
}

QModelIndex DFontPreviewListView::currModelIndex()
{
    return m_currModelIndex;
}

DFontPreviewItemData DFontPreviewListView::currModelData()
{
    QVariant varModel = m_fontPreviewProxyModel->data(m_currModelIndex, Qt::DisplayRole);
    DFontPreviewItemData itemData = varModel.value<DFontPreviewItemData>();

    return itemData;
}

DFontPreviewProxyModel *DFontPreviewListView::getFontPreviewProxyModel()
{
    return m_fontPreviewProxyModel;
}

void DFontPreviewListView::removeRowAtIndex(QModelIndex modelIndex)
{
    QVariant varModel = m_fontPreviewProxyModel->data(modelIndex, Qt::DisplayRole);
    DFontPreviewItemData itemData = varModel.value<DFontPreviewItemData>();

    m_dbManager->deleteFontInfoByFontId(itemData.strFontId);

    m_fontPreviewProxyModel->removeRow(modelIndex.row(), modelIndex.parent());
}
#include "dfontinfodialog.h"
#include "dfontpreviewitemdef.h"

#include <QFileInfo>
#include <QFontMetrics>
#include <QTextBlock>
#include <QVBoxLayout>

#include <DApplication>

DFontInfoDialog::DFontInfoDialog(DFontPreviewItemData *fontInfo, QWidget *parent)
    : DDialog(parent)
    , m_fontInfo(fontInfo)
{
    initUI();
    initConnections();
}

void DFontInfoDialog::initUI()
{
    setFixedSize(QSize(DEFAULT_WINDOW_W, DEFAULT_WINDOW_H));

    // ToDo:
    //    Need localize the string

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setContentsMargins(10, 0, 10, 10);
    mainLayout->setSpacing(0);

    m_mainFrame = new DFrame(this);
    m_mainFrame->setFrameShape(DFrame::Shape::NoFrame);
    m_mainFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Font logo
    m_fontLogo = new DLabel(this);
    m_fontLogo->setFixedSize(QSize(128, 128));
    m_fontLogo->setPixmap(QPixmap(":/images/font-info-logo.svg"));

    // Font file name
    m_fontFileName = new DLabel(this);
    m_fontFileName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_fontFileName->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_fontFileName->setFixedHeight(18);
    QFont fileNameFont;
    fileNameFont.setPixelSize(12);
    m_fontFileName->setFont(fileNameFont);
    m_fontFileName->setText("SourceHanSansSC-ExtraLight");

    /**************************Basic info panel****BEGIN*******************************/
    m_basicInfoFrame = new DFrame(this);
    m_basicInfoFrame->setFrameShape(DFrame::Shape::NoFrame);
    m_basicInfoFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGridLayout *baseicInfoLayout = new QGridLayout();
    baseicInfoLayout->setAlignment(Qt::AlignTop /*| Qt::AlignVCenter*/);
    baseicInfoLayout->setContentsMargins(10, 5, 0, 0);
    baseicInfoLayout->setSpacing(8);

    DLabel *panelName = new DLabel(this);
    panelName->setFixedSize(QSize(60, 20));
    QFont panelNameFont;
    // panelNameFont.setBold(true);
    panelNameFont.setPixelSize(14);
    panelName->setFont(panelNameFont);
    panelName->setText(DApplication::translate("FontDetailDailog", "Basic Information"));

    // Style Row
    QFont basicInfoFont;
    basicInfoFont.setPixelSize(12);

    DLabel *styleName = new DLabel(this);
    styleName->setFixedSize(QSize(60, 18));
    styleName->setFont(basicInfoFont);
    styleName->setText(DApplication::translate("FontDetailDailog", "Style"));

    QHBoxLayout *styleLayout = new QHBoxLayout();
    styleLayout->setSpacing(0);

    m_fontSytleName = new DLabel(this);
    m_fontSytleName->setFixedHeight(18);
    m_fontSytleName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_fontSytleName->setFont(basicInfoFont);
    m_fontSytleName->setText("Regular");
    styleLayout->addSpacing(4);
    styleLayout->addWidget(m_fontSytleName);

    // Type Row
    DLabel *typeName = new DLabel(this);
    typeName->setFixedSize(QSize(60, 18));
    typeName->setFont(basicInfoFont);
    typeName->setText(DApplication::translate("FontDetailDailog", "Type"));

    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->setSpacing(0);

    m_fontTypeName = new DLabel(this);
    m_fontTypeName->setFixedHeight(18);
    m_fontTypeName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_fontTypeName->setFont(basicInfoFont);
    m_fontTypeName->setText("True Type");
    typeLayout->addSpacing(4);
    typeLayout->addWidget(m_fontTypeName);

    // Version row
    DLabel *versionName = new DLabel(this);
    versionName->setFixedSize(QSize(60, 18));
    versionName->setFont(basicInfoFont);
    versionName->setText(DApplication::translate("FontDetailDailog", "Version"));

    m_fontVersion = new QTextEdit(this);
    m_fontVersion->setFixedHeight(72);
    m_fontVersion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_fontVersion->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_fontVersion->setFrameShape(DFrame::Shape::NoFrame);
    m_fontVersion->setContentsMargins(0, 0, 0, 0);
    m_fontVersion->setFont(basicInfoFont);
    m_fontVersion->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_fontVersion->setText(
        "Copyright 2014~2015 Adobe Syste-ms Incorporated (http://www.adob.com/), with Reserved "
        "Font Name cc Source.");

    // Description row
    DLabel *despName = new DLabel(this);
    despName->setFixedSize(QSize(60, 18));
    despName->setFont(basicInfoFont);
    despName->setText(DApplication::translate("FontDetailDailog", "Description"));

    /* Text in QTextEdit have about 4px left-margin,that can't
     * be removed, so add a QHBoxLayout in row tow and add 4px space
     * to align row two
     */
    QHBoxLayout *depsLayout = new QHBoxLayout();
    depsLayout->setSpacing(0);

    m_fontDescription = new DLabel(this);
    m_fontDescription->setFixedHeight(18);
    m_fontDescription->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_fontDescription->setFont(basicInfoFont);
    m_fontDescription->setText(DApplication::translate("FontDetailDailog", "Unkonw"));

    depsLayout->addSpacing(4);
    depsLayout->addWidget(m_fontDescription, 1);

    baseicInfoLayout->addWidget(panelName, 0, 0);
    baseicInfoLayout->addWidget(styleName, 1, 0);
    baseicInfoLayout->addLayout(styleLayout, 1, 1);
    baseicInfoLayout->addWidget(typeName, 2, 0);
    baseicInfoLayout->addLayout(typeLayout, 2, 1);
    baseicInfoLayout->addWidget(versionName, 3, 0, Qt::AlignTop);
    baseicInfoLayout->addWidget(m_fontVersion, 3, 1);
    baseicInfoLayout->addWidget(despName, 4, 0);
    baseicInfoLayout->addLayout(depsLayout, 4, 1);

    m_basicInfoFrame->setLayout(baseicInfoLayout);
    /**************************Basic info panel****END*******************************/

    // Add childs to main layout
    mainLayout->addSpacing(58);
    mainLayout->addWidget(m_fontLogo, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(16);
    mainLayout->addWidget(m_fontFileName);
    mainLayout->addSpacing(45);
    mainLayout->addWidget(m_basicInfoFrame);

    m_mainFrame->setLayout(mainLayout);

    // Update font info to UI
    updateFontInfo();
#ifdef FTM_DEBUG_LAYOUT_COLOR
    m_mainFrame->setStyleSheet("background: red");
    m_fontLogo->setStyleSheet("background: silver");
    m_fontFileName->setStyleSheet("background: silver");
    m_basicInfoFrame->setStyleSheet("background: yellow");
    panelName->setStyleSheet("background: blue");
    versionName->setStyleSheet("background: green");
    m_fontVersion->setStyleSheet("background: green");
    m_fontDescription->setStyleSheet("background: green");
#endif
}

void DFontInfoDialog::initConnections() {}

void DFontInfoDialog::resizeEvent(QResizeEvent *event)
{
    DDialog::resizeEvent(event);

    m_mainFrame->setFixedSize(event->size().width(), event->size().height());
}

void DFontInfoDialog::updateFontInfo()
{
    if (nullptr != m_fontInfo) {
        if (!m_fontInfo->pFontInfo->filePath.isEmpty()) {
            QFileInfo file(m_fontInfo->pFontInfo->filePath);
            m_fontFileName->setText(file.fileName());
        }

        if (!m_fontInfo->pFontInfo->styleName.isEmpty()) {
            m_fontSytleName->setText(m_fontInfo->pFontInfo->styleName);
        }

        if (!m_fontInfo->pFontInfo->type.isEmpty()) {
            m_fontTypeName->setText(m_fontInfo->pFontInfo->type);
        }

        if (!m_fontInfo->pFontInfo->copyright.isEmpty()) {
            m_fontVersion->setText(m_fontInfo->pFontInfo->copyright);
        }

        if (!m_fontInfo->pFontInfo->description.isEmpty()) {
            QString elideString = m_fontDescription->fontMetrics().elidedText(
                m_fontInfo->pFontInfo->description, Qt::ElideRight, 200);
            m_fontDescription->setText(elideString);
        }
    }
}

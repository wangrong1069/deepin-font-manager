#ifndef ATTRSCROLLWIDGET_H
#define ATTRSCROLLWIDGET_H

#include <map>
#include <DFrame>
#include <DWidget>
#include <DLabel>
#include <QGridLayout>
#include <QTextLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <dfontpreviewitemdef.h>//
#include <DApplication>
#include "dfontbasedialog.h"
#include "dfontbasedialog.h"

#include <QResizeEvent>
#include <QPointF>
#include <DFrame>
#include <DLabel>
#include <QTextEdit>
#include <DWidget>
DWIDGET_USE_NAMESPACE

/**
 * @brief The AttrScrollWidget class
 * @brief   属性显示
 */


class dfontinfoscrollarea: public DFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(dfontinfoscrollarea)

public:
    explicit dfontinfoscrollarea(DFontPreviewItemData *pData, DWidget *parent  = nullptr);
     QString elideText(const QString &text, const QFont &font, int nLabelSize);//
     void updateText();

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;

private:
    void createLabel(QGridLayout *layout, const int &index, const QString &objName, const QString &sData);


private:
    DFrame *addTitleFrame(const QString &sData,const QString &objName);
    int m_leftminwidth;

    DFontPreviewItemData *m_fontInfo;//字体信息

    void paintEvent(QPaintEvent *event) override;

    std::map<QLabel* , QString> pLabelMap;//存储信息label键值对
};

#endif // ATTRSCROLLWIDGET_H
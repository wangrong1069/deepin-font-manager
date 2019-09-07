#ifndef DFONTPREVIEWPROXYMODEL_H
#define DFONTPREVIEWPROXYMODEL_H

#include <QSortFilterProxyModel>

class DFontPreviewProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit DFontPreviewProxyModel(QObject *parent = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void setFilterGroup(int filterGroup);

    void setUseSystemFilter(bool useSystemFilter);

    bool isCustomFilterAcceptsRow(const QModelIndex &modelIndex) const;

    bool isFontNameContainsPattern(QString fontName) const;

    bool isChineseFont(QString fontFilePath) const;

    bool isMonoSpaceFont(QString fontFilePath) const;

    void setFilterFontNamePattern(const QString &pattern);

signals:
    void onFilterFinishRowCountChanged(int rowCount) const;

private:
    int m_filterGroup;
    bool m_useSystemFilter;
    QString m_fontNamePattern;

    QStringList m_chineFontPathList;
    QStringList m_monoSpaceFontPathList;
};

#endif  // DFONTPREVIEWPROXYMODEL_H
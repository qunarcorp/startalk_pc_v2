//
// Created by cc on 18-12-21.
//

#ifndef QTALK_V2_STAFFSTRUCTURE_H
#define QTALK_V2_STAFFSTRUCTURE_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>

/**
 * 显示组织架构 组织节点
 */
 class CreatGroupPoupWnd;
class StaffStructure : public QFrame
{
    Q_OBJECT
public:
    explicit StaffStructure(QWidget* parent = nullptr);
    ~StaffStructure() override;

public:
    void setData(const QString& parentStr, const QString& name, int num);

signals:
    void creatGroupSignal(const QString&, const QString& );

private:
    QLabel*      _pParentLabel;
    QLabel*      _pCurrentLabel;
    QLabel*      _pCountLabel;
    QPushButton* _pCreatGroupBtn;
    CreatGroupPoupWnd* _pCreatGroupPoupWnd;

private:
    QString      _structureStr;
    int          _structureCount;
};


#endif //QTALK_V2_STAFFSTRUCTURE_H

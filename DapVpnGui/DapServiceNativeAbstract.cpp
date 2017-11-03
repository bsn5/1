#include <QtDebug>
#include "DapServiceNativeAbstract.h"

/**
 * @brief DapServiceNativeAbstract::DapServiceNativeAbstract
 */
DapServiceNativeAbstract::DapServiceNativeAbstract(){}

/**
 * @brief DapServiceNativeAbstract::init
 */
void DapServiceNativeAbstract::init()
{
    qInfo()<<"::init()";
    checkInstallation();
}

/**
 * @brief DapServiceNativeAbstract::rootExecute
 * @param a_cmd
 * @return
 */
bool DapServiceNativeAbstract::rootExecute(const QString & a_cmd)
{
    qWarning() << "rootExecute() no implementation";
    qDebug()   << "cmd is: " << a_cmd;
    return false;
}

/**
 * @brief DapServiceNativeAbstract::rootObtain
 */
void DapServiceNativeAbstract::rootObtain()
{
    qWarning() << "rootObtain() no implementation";
}

/**
 * @brief DapServiceNativeAbstract::rootLeft
 */
void DapServiceNativeAbstract::rootLeft()
{
    qWarning() << "rootLeft() no implementation";
}

/**
 * @brief DapServiceNativeAbstract::rootInstallService
 */
void DapServiceNativeAbstract::rootInstallService()
{
    qWarning() << "rootInstallService() no implementation";
}

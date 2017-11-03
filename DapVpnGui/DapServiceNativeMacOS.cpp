#include <QtDebug>
#include "DapServiceNativeMacOS.h"

DapServiceNativeMacOS::DapServiceNativeMacOS()
{

}

/**
 * @brief DapServiceNativeMacOS::checkInstallation
 */
void DapServiceNativeMacOS::checkInstallation()
{

    qDebug() << "[DapServiceNativeMacOS] checkInstallation()";
    if( ::system(QString("netstat -anv | grep LISTEN | grep '127.0.0.1.22142' >/dev/null").toLatin1().constData() ) == 0 ){
        qDebug() << "[DapServiceNativeMacOS] 22142 port is open on 127.0.0.1";
        /*rootObtain();
        rootExecute("echo 1234 > /var/tmp/UNSW_root.touched");
        rootLeft();*/
        /*::system(QString("/usr/bin/osascript -e 'set windowTitle to \"UNSWall requires priveledge access to initialize the secure channel\"\n do shell script \"%1\" with administrator privileges'")
                 .arg(QString("sudo cp -f /Applications/%1.app/Contents/Resources/com.ncoded.SapVPNService.plist /Library/LaunchDaemons/;"
                              "sudo chown root /Library/LaunchDaemons/com.ncoded.SapVPNService.plist;"
                              "sudo chmod 600 /Library/LaunchDaemons/com.ncoded.SapVPNService.plist;"
                              "sudo launchctl unload -w /Library/LaunchDaemons/com.ncoded.SapVPNService.plist;"
                              "sudo launchctl load -w /Library/LaunchDaemons/com.ncoded.SapVPNService.plist;"
                              "sudo launchctl start com.ncoded.SapVPNService"
                              )
                      .arg(SAP_BRAND)).toLatin1().constData() );*/
    }else{
        qDebug() << "[DapServiceNativeMacOS] Have to install daemon and kext";
       /* rootObtain();
        rootExecute("echo 1234 > ~/UNSW_ui.touched");

        rootExecute(QString("cp -f /Applications/%1.app/Contents/Resources/com.ncoded.UnswService.plist /Library/LaunchDaemons/;"
                     "chown root /Library/LaunchDaemons/com.ncoded.UnswService.plist;"
                     "chmod 600 /Library/LaunchDaemons/com.ncoded.UnswService.plist;"
                     "launchctl unload -w /Library/LaunchDaemons/com.ncoded.UnswService.plist;"
                     "launchctl load -w /Library/LaunchDaemons/com.ncoded.UnswService.plist;"
                     "launchctl start com.ncoded.SapVPNService"
                     ).arg(SAP_BRAND));
        rootLeft();*/

        ::system(QString("/usr/bin/osascript -e 'set windowTitle to \"UNSW requires priveledge access to initialize the secure channel\"\n do shell script \"%1\" with administrator privileges'")
                 .arg(QString("sudo cp -f /Applications/%1.app/Contents/Resources/com.ncoded.UnswService.plist /Library/LaunchDaemons/;"
                              "sudo chown root /Library/LaunchDaemons/com.ncoded.UnswService.plist;"
                              "sudo chmod 600 /Library/LaunchDaemons/com.ncoded.UnswService.plist;"
                              "sudo launchctl unload -w /Library/LaunchDaemons/com.ncoded.UnswService.plist;"
                              "sudo launchctl load -w /Library/LaunchDaemons/com.ncoded.UnswService.plist;"
                              "sudo launchctl start com.ncoded.UnswService"
                              )
                      .arg(DAP_BRAND)).toLatin1().constData() );
    }
}

/**
 * @brief DapServiceNativeMacOS::restartService
 */
void DapServiceNativeMacOS::restartService()
{
    qWarning() << "[DapServiceNativeMacOS] restartService() No implementation";
}

/**
 * @brief DapServiceNativeMacOS::rootObtain
 */
void DapServiceNativeMacOS::rootObtain()
{
/*
    qDebug() <<"[DapServiceNativeMacOS] rootObtain";

    if (AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authRef) != errAuthorizationSuccess) {
        qWarning()<<"[DapServiceNativeMacOS] Can't init authorization context!!";
        return;
    }

    AuthorizationItem item = {
        kAuthorizationRightExecute, 0, 0, 0
    };
    AuthorizationRights rights = { 1, &item };
    const AuthorizationFlags flags = kAuthorizationFlagDefaults
                                   | kAuthorizationFlagInteractionAllowed
                                   | kAuthorizationFlagPreAuthorize
                                   | kAuthorizationFlagExtendRights;

    if (AuthorizationCopyRights(authRef, &rights, kAuthorizationEmptyEnvironment, flags, 0) != errAuthorizationSuccess) {
        AuthorizationFree(authRef, kAuthorizationFlagDestroyRights);
        qWarning()<<"[DapServiceNativeMacOS] Can't copy authorization rights!!";
        return;
    }
    isAuthRoot=true;
*/
}

/**
 * @brief DapServiceNativeMacOS::rootLeft
 */
void DapServiceNativeMacOS::rootLeft()
{
/*
    qDebug() <<"[DapServiceNativeMacOS] rootLeft";
    if(isAuthRoot){
        AuthorizationFree(authRef, kAuthorizationFlagDestroyRights);
        isAuthRoot=false;
        qWarning() <<"[DapServiceNativeMacOS] rootLeft() called successfully";
    }else
        qWarning() <<"[DapServiceNativeMacOS] rootLeft() called but we're not in root state";
*/
}

/**
 * @brief DapServiceNativeMacOS::rootInstallService
 */
void DapServiceNativeMacOS::rootInstallService()
{
 /*   CFErrorRef  cfError;
    bool result =false;
    result = (bool) SMJobBless(kSMDomainSystemLaunchd, (CFStringRef)label, authRef, &cfError);
    if (!result) {
        error = CFBridgingRelease(cfError);
    }*/
}

/**
 * @brief DapServiceNativeMacOS::rootExecute
 * @param a_cmd
 * @return
 */
bool DapServiceNativeMacOS::rootExecute(const QString & a_cmd)
{
/*
    qDebug() <<"[DapServiceNativeMacOS] rootExecute: "<<a_cmd;

    char r_program[]="/bin/bash";
    char *r_arg1 = ::strdup("-c");
    char *r_arg2 = reinterpret_cast<char*>(::malloc(a_cmd.length()+3));
    r_arg2[0]='\'';
    ::memcpy(r_arg2+1,a_cmd.toLatin1().constData(),a_cmd.length());
    r_arg2[1+a_cmd.length()]='\'';
    r_arg2[2+a_cmd.length()]=0;

    std::vector<char *> r_args;
    r_args.push_back(r_arg1);
    r_args.push_back( r_arg2  );

    OSStatus status = AuthorizationExecuteWithPrivileges(authRef, r_program, kAuthorizationFlagDefaults, &r_args[0], 0);

    delete[] r_arg2;
    ::free(r_arg1);
    return status == errAuthorizationSuccess;
*/
}

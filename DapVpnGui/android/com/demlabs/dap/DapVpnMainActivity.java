package com.demlabs.dap;
import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;
import android.content.Intent;
import android.os.Bundle;
import android.net.VpnService;
import android.util.Log;
import com.demlabs.dap.DapVpnServiceNative;
import com.demlabs.dap.DapVpnService;

public class DapVpnMainActivity extends QtActivity
{
    public final static String LOG_TAG="DapVpnMainActivity";
    @Override
    public void onCreate(Bundle savedInstanceState)
    {

        super.onCreate(savedInstanceState);
    }

    String m_addr,m_gw;
    int m_socketUpstream;
    public void startDapVpnServiceNative(String a_addr, String a_gw, int a_socketUpstream)
    {
        Log.i(LOG_TAG, "Start DapVpnServiceNative() require permissions");
        m_addr=a_addr;
        m_gw=a_gw;
        m_socketUpstream = a_socketUpstream;
        Intent intentPermissions = VpnService.prepare(getApplicationContext());
        if (intentPermissions != null) {
            startActivityForResult(intentPermissions, 123);
        }else{
            Log.i(LOG_TAG, "startDapVpnNativeService() Permissions are present");
            Intent newIntent=new Intent(getApplicationContext(), DapVpnServiceNative.class);
            newIntent.putExtra(DapVpnServiceNative.EXTRA_SOCKET,m_socketUpstream);
            newIntent.putExtra(DapVpnServiceNative.EXTRA_ADDR,m_addr);
            newIntent.putExtra(DapVpnServiceNative.EXTRA_GW,m_gw);

            getApplicationContext().startService(newIntent);
        }
    }

    public void stopDapVpnServiceNative()
    {
        Log.i(LOG_TAG, "Stop DapVpnServiceNative()");
        Intent newIntent=new Intent(getApplicationContext(), DapVpnServiceNative.class);
        getApplicationContext().stopService(newIntent);
    }


    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (resultCode == RESULT_OK) {
            switch(requestCode){
                case 123:{
                    Log.i(LOG_TAG, "onActivityResult() Permissions are granted");
                    Intent newIntent=new Intent(getApplicationContext(), DapVpnServiceNative.class);
                    newIntent.putExtra(DapVpnServiceNative.EXTRA_SOCKET,m_socketUpstream);
                    newIntent.putExtra(DapVpnServiceNative.EXTRA_ADDR,m_addr);
                    newIntent.putExtra(DapVpnServiceNative.EXTRA_GW,m_gw);
                    getApplicationContext().startService(newIntent);
                } break;
                default: Log.i(LOG_TAG,"onActivityResult() returned OK with request code "+String.valueOf(requestCode));
            }
        }else{
            switch(requestCode){
                case 123:{
                    Log.w(LOG_TAG, "onActivityResult() Permissions are not granted");

                } break;
                default: Log.w(LOG_TAG,"onActivityResult() returned non OK with request code "+String.valueOf(requestCode));
            }

        }
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }
}

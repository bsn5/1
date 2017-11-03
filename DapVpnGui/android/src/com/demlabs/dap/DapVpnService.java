package com.demlabs.dap;
import org.qtproject.qt5.android.bindings.QtService;
import org.qtproject.qt5.android.bindings.QtApplication;
import android.content.Context;
import android.content.Intent;
import android.app.Service;
import android.content.res.Configuration;
import android.os.IBinder;

import android.util.Log;

public class DapVpnService extends QtService
{
        private final static String LOG_TAG="DapVpnService";

/*        @Override
        protected void onCreateHook() {
            Log.i(LOG_TAG,"onCreateHook()");
            super.onCreateHook();
        }


        @Override
        public void onCreate() {
            Log.i(LOG_TAG, "onCreate()");
            super.onCreate();
        }

        @Override
        public IBinder onBind(Intent intent)
        {
            QtApplication.InvokeResult res = QtApplication.invokeDelegate(intent);
            if (res.invoked){
                Log.i(LOG_TAG,"onBind() Delegate call invoked");
                return (IBinder)res.methodReturns;
            }else{
                Log.e(LOG_TAG,"onBind() Delegate call  is not invoked");
                return null;
            }
        }


        @Override
        public int onStartCommand(Intent intent, int flags, int startId)
        {
           Log.i(LOG_TAG, "onStartCommand()");
           QtApplication.InvokeResult res = QtApplication.invokeDelegate(intent, flags, startId);
           if (res.invoked){
               Log.i(LOG_TAG,"onStartCommand() Delegate call invoked");
               return (Integer) res.methodReturns;
           }else{
               Log.e(LOG_TAG,"onStartCommand() Delegate call  is not invoked");
               return super.onStartCommand(intent, flags, startId);
           }
         }*/


        public static void restartService(Context ctx)
	{
                Log.i("DapVpnService", "Restarting... DapVPN core service");
               // ctx.stopService(new Intent(ctx, DapVpnService.class));
                ctx.startService(new Intent(ctx, DapVpnService.class));
                Log.i("DapVpnService", "Restarted DapVPN core service");

	}
}



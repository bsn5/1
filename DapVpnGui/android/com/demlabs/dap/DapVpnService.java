package com.demlabs;
import org.qtproject.qt5.android.bindings.QtService;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class DapVpnService extends QtService
{
        @Override
        public void onCreate() {
            Log.i("DapVpnService", "onCreate()");
            super.onCreate();
        }

        public static void restartService(Context ctx)
	{
                Log.i("DapVpnService", "Restart DapVPN core service");
                ctx.stopService(new Intent(ctx, DapVpnService.class));
                ctx.startService(new Intent(ctx, DapVpnService.class));
	}
}



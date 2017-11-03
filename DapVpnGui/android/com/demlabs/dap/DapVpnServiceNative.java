package com.demlabs.dap;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.util.Log;
import android.app.Service;
import android.net.VpnService;
import android.os.ParcelFileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.channels.DatagramChannel;
import java.net.InetSocketAddress;
import android.net.VpnService.Builder;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import java.io.BufferedReader;
import java.io.IOException;
import java.lang.NullPointerException;

import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import android.net.LocalSocket;
import android.net.LocalServerSocket;
import android.net.LocalSocketAddress;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.net.InetAddress;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Message;
import android.app.Activity;
import java.lang.Thread;

import android.app.Notification;
import android.app.NotificationManager;
import android.view.View;
import android.view.View.OnClickListener;
import java.io.PrintWriter;
import java.io.*;
import java.io.DataOutputStream;
import java.nio.ByteBuffer;


public class DapVpnServiceNative extends VpnService {
    // VPN Connect THREAD
    public static final String LOG_TAG = "DapVpnServiceNative";
    private ParcelFileDescriptor mInterface;
    private NotificationManager mNM;
    public static final String EXTRA_ADDR="com.demlabs.dap.dapvpnservicenative.extra_addr";
    public static final String EXTRA_GW="com.demlabs.dap.dapvpnservicenative.extra_gw";
    public static final String EXTRA_SOCKET="com.demlabs.dap.dapvpnservicenative.extra_socket";

    public static final String BROADCAST_ACTION =
            "com.demlabs.dap.dapvpnservicenative.BROADCAST";
    //a. Configure a builder for the interface.
    Builder builder = new Builder();


    ServerSocket socketTun = null;
    Socket socketServiceCore = new Socket();

    int m_socketUpstream;
    int m_socketTun;
    String m_tunAddress="";
    String m_tunGW="";
    Thread thCore = null;


    @Override
    public void onRevoke (){
        super.onRevoke();
        Log.i(LOG_TAG, "onRevoke() invoked");
        stopForeground(0);
        if(thCore != null ){
            thCore.interrupt();
            thCore=null;
        }
    }

    @Override
    public void onDestroy() {
        Log.i(LOG_TAG, "onDestroy() invoked");
        if(thCore != null ){
            thCore.interrupt();
            thCore=null;
        }
        super.onDestroy();
    }


  // Services interface
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(LOG_TAG, "onStartCommand() ");
        if(intent == null ){
            Log.e(LOG_TAG,"Null intent is not expected here");
            return -1;
        } else if(intent.getExtras() == null ){
            Log.e(LOG_TAG,"NULL extras that is also not expected here");
            return -1;
        }
        m_socketUpstream=intent.getExtras().getInt(EXTRA_SOCKET);
        m_tunAddress = intent.getExtras().getString(EXTRA_ADDR);
        m_tunGW = intent.getExtras().getString(EXTRA_GW);

        Log.i(LOG_TAG, "socketTun: bind to address "+m_tunAddress+" gateway "+m_tunGW);
        builder.setMtu(1500);
        builder.addAddress(m_tunAddress, 32);
        builder.addDnsServer(m_tunGW);
        builder.addRoute("0.0.0.0", 0);
        builder.setSession("DiveVPN");
        mInterface=builder.establish();

        protect(m_socketUpstream);

        if(mInterface == null){
            Log.e(LOG_TAG, "socketTun: Can't establish VPN session: application is not prepared");
            return -1;
        }else{
            Log.i(LOG_TAG, "socketTun: VPN Session established");
            m_socketTun=mInterface.detachFd();
            thCore = new Thread() {
                public void run(){
                    try{
                        Log.i(LOG_TAG, "Connecting to the core service...");
                        Socket sc = new Socket("127.0.0.1",23434);
                        if(sc.isConnected()){
                           Log.i(LOG_TAG, "Connected to the core service.");
                           sc.getOutputStream().write( ("route_all "+String.valueOf(m_socketTun)+"\n").getBytes() );
                           Log.i(LOG_TAG, "VPN socket "+String.valueOf(m_socketTun)+" is passed to the core service");
                           sc.close();
                        }else{
                            Log.i(LOG_TAG, "Problems! Can't connect to the core service!");
                        }
                    }catch (SecurityException e) {
                        e.printStackTrace();
                        Log.i(LOG_TAG, "onStartCommand() SecurityException: " + e.toString());
                    } catch (IllegalArgumentException e) {
                        e.printStackTrace();
                        Log.i(LOG_TAG, "onStartCommand() IllegalArgumentException: " + e.toString());
                    } catch (IOException e) {
                        e.printStackTrace();
                        Log.i(LOG_TAG, "onStartCommand() IOException: " + e.toString());
                    } catch (NullPointerException	e) {
                        e.printStackTrace();
                        Log.i(LOG_TAG, "onStartCommand() NullPointerException: " + e.toString());
                    } catch (Exception e) {
                      // Catch any exception
                      e.printStackTrace();
                      Log.i(LOG_TAG, "onStartCommand() Exception: " + e.toString());
                    }
                    thCore=null;
                }};
            //startForeground(1, new Notification.Builder(this)
            //        .build());
            Log.i(LOG_TAG, "onStartCommand() Foreground notifier started");
            thCore.start();

            return START_STICKY;
         }
    }
}



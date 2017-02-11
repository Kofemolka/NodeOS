package com.shop.kofe.openhabnotify;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

/**
 * Created by Kofemolka on 2/11/2017.
 */

public class BootReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {

        {
            Intent myIntent = new Intent(context, FirebaseIDService.class);
            context.startService(myIntent);
        }
        {
            Intent myIntent = new Intent(context, OpenHabFMS.class);
            context.startService(myIntent);
        }

    }
}
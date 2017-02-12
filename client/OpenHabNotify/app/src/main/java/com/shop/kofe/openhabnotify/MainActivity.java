package com.shop.kofe.openhabnotify;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;
import com.google.firebase.iid.FirebaseInstanceId;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        DBHandler db = new DBHandler(this);
        Items = db.getAllNotifications();
        Adapter = new NotifycationItemAdapter(this, (ArrayList<Notification>)Items);
        Adapter.setNotifyOnChange(true);

        ListView listView = (ListView) findViewById(R.id.lvNotifications);
        listView.setAdapter(Adapter);
    }

    public void DeleteAll(View view)
    {
        DBHandler db = new DBHandler(this);
        db.deleteAll();
        Items.clear();
        Adapter.notifyDataSetChanged();
    }

    public static List<Notification> Items;
    public static NotifycationItemAdapter Adapter;
}

package com.shop.kofe.openhabnotify;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

/**
 * Created by Kofemolka on 2/12/2017.
 */

public class DBHandler extends SQLiteOpenHelper {
    // All Static variables
    // Database Version
    private static final int DATABASE_VERSION = 1;

    // Database Name
    private static final String DATABASE_NAME = "notificationDB";

    // Contacts table name
    private static final String TABLE_NOTIFICATIONS = "notifications";

    // Contacts Table Columns names
    private static final String KEY_ID = "id";
    private static final String KEY_TITLE = "title";
    private static final String KEY_MSG = "message";
    private static final String KEY_TIME = "timestamp";

    public DBHandler(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    // Creating Tables
    @Override
    public void onCreate(SQLiteDatabase db) {
        String CREATE_NOTIFICATION_TABLE = "CREATE TABLE " + TABLE_NOTIFICATIONS + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_TITLE + " TEXT,"
                + KEY_MSG + " TEXT," + KEY_TIME + " INTEGER " + ")";
        db.execSQL(CREATE_NOTIFICATION_TABLE);
    }

    // Upgrading database
    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // Drop older table if existed
        db.execSQL("DROP TABLE IF EXISTS " + TABLE_NOTIFICATIONS);

        // Create tables again
        onCreate(db);
    }

    /**
     * All CRUD(Create, Read, Update, Delete) Operations
     */

    // Adding new contact
    void addNotification(Notification notification) {
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_TITLE, notification.Title);
        values.put(KEY_MSG, notification.Message);
        values.put(KEY_TIME, notification.Timestamp.getTime());

        // Inserting Row
        db.insert(TABLE_NOTIFICATIONS, null, values);
        db.close(); // Closing database connection
    }

    // Getting single contact
    /*Contact getContact(int id) {
        SQLiteDatabase db = this.getReadableDatabase();

        Cursor cursor = db.query(TABLE_CONTACTS, new String[] { KEY_ID,
                        KEY_NAME, KEY_PH_NO }, KEY_ID + "=?",
                new String[] { String.valueOf(id) }, null, null, null, null);
        if (cursor != null)
            cursor.moveToFirst();

        Contact contact = new Contact(Integer.parseInt(cursor.getString(0)),
                cursor.getString(1), cursor.getString(2));
        // return contact
        return contact;
    }*/

    // Getting All Contacts
    public List<Notification> getAllNotifications() {
        List<Notification> notifications = new ArrayList<Notification>();
        // Select All Query
        String selectQuery = "SELECT  * FROM " + TABLE_NOTIFICATIONS;

        SQLiteDatabase db = this.getWritableDatabase();
        Cursor cursor = db.rawQuery(selectQuery, null);

        // looping through all rows and adding to list
        if (cursor.moveToFirst()) {
            do {
                Notification notification = new Notification();
                notification.ID = Integer.parseInt(cursor.getString(0));
                notification.Title = cursor.getString(1);
                notification.Message = cursor.getString(2);
                notification.Timestamp = new Date(Long.parseLong(cursor.getString(3)));
                // Adding contact to list
                notifications.add(notification);
            } while (cursor.moveToNext());
        }

        // return contact list
        return notifications;
    }

    /*
    // Updating single contact
    public int updateContact(Contact contact) {
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_NAME, contact.getName());
        values.put(KEY_PH_NO, contact.getPhoneNumber());

        // updating row
        return db.update(TABLE_CONTACTS, values, KEY_ID + " = ?",
                new String[] { String.valueOf(contact.getID()) });
    }
*/
    // Deleting single contact
    public void deleteAll() {
        SQLiteDatabase db = this.getWritableDatabase();
        db.delete(TABLE_NOTIFICATIONS, null, null);
        db.close();
    }

/*
    // Getting contacts Count
    public int getContactsCount() {
        String countQuery = "SELECT  * FROM " + TABLE_CONTACTS;
        SQLiteDatabase db = this.getReadableDatabase();
        Cursor cursor = db.rawQuery(countQuery, null);
        cursor.close();

        // return count
        return cursor.getCount();
    }*/
}

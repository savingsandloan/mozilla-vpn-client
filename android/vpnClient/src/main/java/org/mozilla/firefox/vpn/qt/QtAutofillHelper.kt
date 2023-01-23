package org.mozilla.firefox.vpn.qt

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.Rect
import android.os.Build
import android.util.Log
import android.view.autofill.AutofillManager
import androidx.annotation.RequiresApi
import kotlinx.serialization.decodeFromString
import kotlinx.serialization.json.Json
import org.json.JSONObject


/**
 * Qt Autofill Helper.
 * This class will fake the "Login" View
 * for autofill purposes.
 */
@SuppressLint("StaticFieldLeak")
@RequiresApi(Build.VERSION_CODES.O)
object QtAutofillHelper {

    private val ctx : Context
        get() {
            return VPNActivity.getInstance();
        }
    private val manager by lazy {
        ctx.getSystemService(AutofillManager::class.java)
    }
    private var mView : VPNAuthAutofillView? = null


    /**
     * Will Start a fresh Auto Fill session,
     * inflates a fake view-structure and request's it to be autofilled,
     *
     */
    @JvmStatic
    fun start(data:String){
        if(!manager.isAutofillSupported){
            return;
        }
        val json = JSONObject(data);
        val params = VPNAuthAutofillView.AutoFillParms(
            json.getString("fxa_url"),
            json.getInt("x"),
            json.getInt("y"),
            json.getInt("width"),
            json.getInt("height"),
        );
        mView = VPNAuthAutofillView(ctx,params,this)
        // TODO: we should prefer the call with the bounding box. 
        // Todo: we should A: send the
        val box = Rect(params.x,params.y,params.x+params.width,params.y+params.height)
        manager.notifyViewEntered(mView!!,1, box)
    }

    /**
     * Callback, whenever the autofill finished.
     */
    fun onAutofill(username:String, password:String){
        Log.i("OH BOIII,", "$username, $password")
        onAutofill(JSONObject().apply {
            put("username",username)
            put("password",password)
        }.toString())
    }

    external fun onAutofill(data:String)

    /*
     * Updates the Fake View's values and commits them
     * - This should enable password manages to show a
     * "Did you want to save this?" Prompt for next time :3
     */
    @JvmStatic
    fun updateAndCommit(username: String,password: String){
        mView?.let {
            updateAndCommit(username,password)
            // TODO: check if the viewstructure is re-inflated
            // or we need to keep the values up to date. 
            // Gecko-View uses this aswell
            manager.notifyValueChanged(it)
        }
        manager.commit()
    }
    /*
     * Cancel's the current autofill session.
     */
    @JvmStatic
    fun cancel(){
        manager.cancel()
        mView=null
    }

}
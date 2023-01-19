package org.mozilla.firefox.vpn.qt

import android.content.Context
import android.os.Build
import android.util.SparseArray
import android.view.View
import android.view.ViewStructure
import android.view.autofill.AutofillValue
import androidx.annotation.RequiresApi
import androidx.core.util.isEmpty


@RequiresApi(Build.VERSION_CODES.O)
class VPNAuthAutofillView(context: Context?,qtContext: AutoFillParms, parent : QtAutofillHelper) : View(context) {
    var mParams = qtContext
    val mParent = parent;

    @kotlinx.serialization.Serializable
    data class AutoFillParms(
        val fxa_url:String,
    )

    override fun autofill(values: SparseArray<AutofillValue>) {
        super.autofill(values)
        if (values.isEmpty()) {
            return;
        }
        try {
            val password = values.get(0);
            val username = values.get(1);

            if(!password.isText || !username.isText){
                // False values >:c
                mParent.onAutofill(password.toString(),username.toString())
            }
        }catch (e:Exception){
            //cool don't care.
        }
    }


    override fun autofill(value: AutofillValue?) {
        super.autofill(value)
        if (value == null) {
            return;
        }
        // Note: this should not be called

    }

    var mUsernameField :ViewStructure? = null;
    var mPasswordField :ViewStructure? = null;


    override fun onProvideAutofillVirtualStructure(structure: ViewStructure?, flags: Int) {
        super.onProvideAutofillVirtualStructure(structure, flags)
        if(structure == null){
            return;
        }
        // Reserve storage for 2 New childs:
        structure.addChildCount(2)

        // Set the Container to the FXA domain, so it knows
        // to look for those passwords. (Thanks gecko team)
        structure.setWebDomain(mParams.fxa_url);

        val passwordField = structure.newChild(0)
        passwordField.setWebDomain(mParams.fxa_url)
        passwordField.setAutofillType(AUTOFILL_TYPE_TEXT)
        passwordField.setDataIsSensitive(true)
        passwordField.setFocused(true)
        structure.autofillId?.let { passwordField.setAutofillId(it) }
        passwordField.setAutofillHints(arrayOf(AUTOFILL_HINT_USERNAME,AUTOFILL_HINT_EMAIL_ADDRESS))

        val usernameField = structure.newChild(1)
        usernameField.setWebDomain(mParams.fxa_url)
        usernameField.setAutofillType(AUTOFILL_TYPE_TEXT)

        usernameField.setDataIsSensitive(true)
        structure.autofillId?.let { usernameField.setAutofillId(it) }
        usernameField.setAutofillHints(arrayOf(AUTOFILL_HINT_PASSWORD))

        mUsernameField=usernameField
        mPasswordField=passwordField
    }


    fun updateFilledValues(username:String, password:String){
        mPasswordField?.let {
            it.text = password
        }
        mUsernameField?.let {
            it.text = username
        }
    }


}


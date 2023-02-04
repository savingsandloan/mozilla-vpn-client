import com.google.android.play.core.appupdate.AppUpdateManagerFactory
import com.google.android.play.core.install.model.UpdateAvailability
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import org.mozilla.firefox.vpn.qt.VPNActivity

object VPNVersionAPI {

    @kotlinx.serialization.Serializable
    data class UpdateResult(
        // Is an update available
        val isUpdateAvailable:Boolean,
        // How important is the update ?
        // 1 -> Low
        // 5 -> SUPER IMPORTANT
        // Note: This can only be set right now by the Google play dev-api
        // so we'd require taskcluster changes until this can be fully used.
        val updateLevel: Int,
        // What is the Bundle version Code
        // Note: this is not the VPN version.
        val versionCode: Int,
    )

    @JvmStatic
    fun startUpdateInfo(){
            val manager = AppUpdateManagerFactory.create(VPNActivity.getInstance());
            val update_info_task = manager.appUpdateInfo;

            update_info_task.addOnCanceledListener {  }
            update_info_task.addOnSuccessListener{
                    val info = UpdateResult(
                        it.updateAvailability() == UpdateAvailability.UPDATE_AVAILABLE,
                        it.updatePriority(),
                        it.availableVersionCode()
                    )
                    onUpdateResult(Json.encodeToString(info));
                }
            update_info_task.addOnCanceledListener {
                onUpdateResult("")
            }
            update_info_task.addOnFailureListener {
                onUpdateResult("")
            }
    }

    external fun onUpdateResult(updateInfo: String);
}
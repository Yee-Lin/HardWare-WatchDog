function changeConfirm(f) {
    if (!confirm('Are you sure to modify the parameters?')) return;
    f.submit();
}

var upgradeProgess = 0;
function upgradeProgressUpdate() {
    document.getElementById("upgrade_progress").value = upgradeProgess;
    upgradeProgess += 20;
    alert(upgradeProgess);
    upgradeTime = setTimeout("upgradeProgressUpdate", 1000);
}

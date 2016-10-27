if (typeof this["Screens"]["SCREEN_1"] == "object")
    this["Screens"]["SCREEN_1"].RefreshScreen();
var curDate = new Date();
var sms_params={};
sms_params.EVENT_ID=curDate.toLocaleTimeString() + curDate.toISOString().substr(19,4);
sms_params.STATUS_TEXT="Station start";
sms_params.STATION_IP=":CLIENT_IP";
RpcTcp.Call_server_proc("StartSms",JSON.stringify(sms_params));
RpcTcp.server_proc_reply.connect(createButtons);
RpcTcp.Call_server_proc("SQLQuery2Json", ["SELECT * FROM VIEW_ACTIVE_BUTTONS WHERE STATION_IP=:CLIENT_IP"]);

function createButtons(jsonresult) {
print("gui client start createButtons");
if (jsonresult.length==0) {return;}
var jsondoc=JSON.parse(jsonresult);
var refresh_flg = false;
for (var i=0;i<jsondoc.length;++i)
    if (typeof jsondoc[i].OBJECT_NAME != "undefined") {
          for (key in jsondoc[i])
               if (jsondoc[i][key] != null)
                  jsondoc[i][key]= jsondoc[i][key].toString().trim();
this.Screens.SCREEN_1.CopyWidget("DEFAULT_CURRENT_STATUS",JSON.stringify(jsondoc[i]));
        refresh_flg=true;
    }
if ((typeof this["Screens"]["SCREEN_1"] == "object") && refresh_flg) {
this["Screens"]["SCREEN_1"].RefreshScreen();
if (typeof this.timer != "object") {
var timer = new QTimer();
timer.interval = 30000;
timer.singleShot = false;
RpcTcp.server_proc_reply.connect(this.UpdateText);
timer.timeout.connect(this.RequestSmsReport);
timer.start();
RequestSmsReport();
}}
//var slot = this.createButtons;
RpcTcp.server_proc_reply.disconnect(this.createButtons);}

function RequestSmsReport() {
RpcTcp.Call_server_proc("SQLQuery2Json", ["SELECT SMS_REPORT, EVENT_ID FROM VIEW_SMS_DEVIVERED WHERE STATION_IP=:CLIENT_IP"]);}
function UpdateText(jsonresult) {
if (jsonresult.length==0) {print("empty jsonresult");return;}
var jsondoc=JSON.parse(jsonresult);
if (jsondoc.length==0) {
print("no current statuses");
if (typeof this.timer == "object") {
timer.deleteLater();
timer.timeout.disconnect(this.RequestSmsReport);
RpcTcp.server_proc_reply.disconnect(this.UpdateText);
}
return;}
for (var i=0;i<jsondoc.length;++i) {
var sms_report=jsondoc[i].SMS_REPORT;
var objname="CURRENT_STATUS_" + jsondoc[i].EVENT_ID;
var curstbtn = this.Screens.SCREEN_1[objname];
if (typeof curstbtn == "object") {
curstbtn.OBJECT_TEXT=curstbtn.OEM_OBJECT_TEXT+" "+sms_report;
this.Screens.SCREEN_1.RefreshButton(curstbtn);}
else
print("No object with name: "+objname);}}

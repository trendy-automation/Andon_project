/*
 * This file is generated and updated by Sencha Cmd. You can edit this file as
 * needed for your application, but these edits will have to be merged by
 * Sencha Cmd when upgrading.
 */

/*
Ext.Loader.setConfig ({
                          enabled: true,
                          paths: {

//                              'Ext.ux.WebSocket': 'app/ExtJS-WebSocket/WebSocket.js' ,
//                              'Ext.ux.WebSocketManager': 'app/ExtJS-WebSocket/WebSocketManager.js',
                              'Ext.ux.SqlSocket': 'app/ExtJS-WebSocket/SqlSocket.js'//,
                              //'AndonPortal.store.Tree' : 'app/store/Tree.js'
                          }
                      });
Ext.require(['AndonPortal.model.Tree']);

Ext.require ([//'Ext.ux.WebSocket',
              //'Ext.ux.WebSocketManager',
              //'Ext.selection.Model',
              'AndonPortal.store.Tree',
              //'AndonPortal.model.Tree',
              'Ext.ux.SqlSocket'
//              'AndonPortal.Application'
             ]);
*/
/*
Ext.application({
    name: 'AndonPortal',
    
    requires: [
        'Ext.window.Toast'
    ],
//    stores: ['Tree'],
    views: [
//        'Buttons',
        'DatePicker',
//        'Form',
        'Grid',
        'Tree',
//        'Image',
//        'ItemSelector',
//        'List',
//        'Panel',
//        'Toolbar',
        'Viewport'
//        'Window'
    ],
                    launch: function () {
                        console.log('app launch function');
                        //Ext.tip.QuickTipManager.init();
                        var qws = Ext.create ('Ext.ux.SqlSocket', {
                                                  id : 'ServerWebSocket',
                                                  //url: "ws://"+location.hostname+":12346",
                                                  //url: "ws://127.0.0.1:12346",
                                                  url: "ws://10.208.110.75:12346",
                                                  listeners: {
                                                      open: function (ws) {
                                                          console.log('SqlSocket opened');
                                                          ws.db.query2json("SELECT JSON_BRANCH FROM TREE_GET_JSONTREE('plant')",
                                                                           function(resp){
                                                                               var JSONdata =[];
                                                                               JSONdata=JSON.parse(resp);
                                                                               //console.log('resp',resp  );
                                                                               //if(JSONdata.length!=0)
                                                                                   //JSONdata = eval(resp);
                                                                               //JSONdata = eval('['+JSONdata[0].JSON_BRANCH+']');
                                                                               JSONdata = JSONdata.map(function(node) {
                                                                                                return eval('['+node.JSON_BRANCH+']')[0];
                                                                                                })
                                                                               //};
                                                                               //console.log('JSONdata',JSONdata);
                                                                               Ext.ux.ajax.SimManager.init({
                                                                                                               delay: 300,
                                                                                                               defaultSimlet: null
                                                                                                           }).register({'/json/treePanel': {
                                                                                                                               data: JSONdata,
                                                                                                                               stype: 'json'
                                                                                                                           }
                                                                                                                       });
                                                                               AndonPortal.store.Tree.load();
                                                                               //treeStore.load();
                                                                               //console.log(' treeStore.getData()', treeStore.getData());
                                                                           }
                                                                           );
                                                      },
                                                      close: function (ws) {
                                                          //clearInterval(refreshIntervalId);
                                                          console.log ('The qwebsocket is closed!');
                                                      }
                                                  }
                                              });

                    },

    autoCreateViewport: 'AndonPortal.view.Viewport',
    */



Ext.application({
    name: 'AndonPortal',

    extend: 'AndonPortal.Application',

    autoCreateViewport: 'AndonPortal.view.Viewport',

    //stores: ['Tree'],
    requires: [
       'Ext.window.Toast'
    ],
    views: [
//        'Buttons',
        'DatePicker',
//        'Form',
        'Grid',
        'Tree',
//        'Image',
//        'ItemSelector',
//        'List',
//        'Panel',
//        'Toolbar',
        'Viewport'
//        'Window'
    ],

    //-------------------------------------------------------------------------
    // Most customizations should be made to AndonPortal.Application. If you need to
    // customize this file, doing so below this section reduces the likelihood
    // of merge conflicts when upgrading to new versions of Sencha Cmd.
    //-------------------------------------------------------------------------
    msg: function(title, format) {
        var s = Ext.String.format.apply(String, Array.prototype.slice.call(arguments, 1));
        Ext.toast(s, title);
    }
});

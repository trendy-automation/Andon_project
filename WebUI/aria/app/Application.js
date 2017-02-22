/**
 * The main application class. An instance of this class is created by app.js when it
 * calls Ext.application(). This is the ideal place to handle application launch and
 * initialization details.
 */
Ext.Loader.setConfig ({
                          enabled: true,
                          paths: {

                              'Ext.ux.WebSocket': 'ExtJS-WebSocket/WebSocket.js' ,
                              'Ext.ux.WebSocketManager': 'ExtJS-WebSocket/WebSocketManager.js',
                              'Ext.ux.qwebchannel': 'ExtJS-WebSocket/qwebchannel.js'
                              //'Ext.ux.SqlSocket': 'ExtJS-WebSocket/SqlSocket.js',
                          }
                      });

Ext.require (['Ext.ux.WebSocket', 'Ext.ux.WebSocketManager']); //,'Ext.ux.SqlSocket'

// Ext.tree.Panel.borrow(Ext.grid.Panel, [
//     'bothCfgCopy',
//     'normalCfgCopy',
//     'lockedCfgCopy'
// ]);


//we want to setup a model and store instead of using dataUrl

Ext.define('AndonPortal.Application', {
               extend: 'Ext.app.Application',

               name: 'AndonPortal',


               stores: [
                   // TODO: add global / shared stores here

               ],

               launch: function () {
                   // TODO - Launch the application
                   Ext.tip.QuickTipManager.init();
                   var qws = Ext.create ('Ext.ux.WebSocket', {
                                             id : 'ServerWebSocket',
                                             url: "ws://"+location.hostname+":12346" ,
                                             listeners: {
                                                 open: function (ws) {
                                                     datajs={};
                                                     ws.db.query2json("SELECT JSON_BRANCH FROM TREE_GET_JSONTREE('plant')",
                                                                      function(resp){

                                                                          Ext.ux.ajax.SimManager.init({
                                                                                                          delay: 300,
                                                                                                          defaultSimlet: null
                                                                                                      }).register({
                                                                                                                      'trees': {
                                                                                                                          data: JSON.parse(resp),
                                                                                                                          stype: 'json'
                                                                                                                      }
                                                                                                                  });
                                                                          treeStore.load();



                                                                      }
                                                                      );
                                                 },
                                                 close: function (ws) {
                                                     clearInterval(refreshIntervalId);
                                                     console.log ('The qwebsocket is closed!');
                                                 }
                                             }
                                         });
               },

               onAppUpdate: function () {
                   Ext.Msg.confirm('Application Update', 'This application has an update, reload?',
                                   function (choice) {
                                       if (choice === 'yes') {
                                           window.location.reload();
                                       }
                                   }
                                   );
               }
           });

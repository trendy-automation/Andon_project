/**
 * The main application class. An instance of this class is created by app.js when it
 * calls Ext.application(). This is the ideal place to handle application launch and
 * initialization details.
 */
Ext.Loader.setConfig ({
                          enabled: true,
                          paths: {

                              'Ext.ux.WebSocket': 'app/ExtJS-WebSocket/WebSocket.js' ,
                              'Ext.ux.WebSocketManager': 'app/ExtJS-WebSocket/WebSocketManager.js',
                              'Ext.ux.SqlSocket': 'app/ExtJS-WebSocket/SqlSocket.js'/*,
                              'AndonPortal.store.Tree': 'app/store/Tree.js'*/
                          }
                      });

Ext.require (['Ext.ux.WebSocket', 'Ext.ux.WebSocketManager','Ext.ux.SqlSocket']);
//Ext.require(['AndonPortal.model.Tree']);

//Ext.require (['AndonPortal.store.Tree'/*,
//              'AndonPortal.model.Tree'*/
//             ]);

Ext.define('AndonPortal.Application', {
               extend: 'Ext.app.Application',
               requires: [
                            'AndonPortal.store.Tree',
//                            'Ext.window.Toast',
//                            'AndonPortal.model.Tree'
               ],
               name: 'AndonPortal',
               stores: ['Tree'],
               views: [
                   'DatePicker',
                   'Grid',
                   'Tree',
                   'Viewport'
               ],
               launch: function () {
                   console.log('Application launch function');
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
                                                                              JSONdata = JSONdata.map(function(node) {
                                                                                               return eval('['+node.JSON_BRANCH+']')[0];
                                                                                               });
                                                                              //console.log('JSONdata',JSONdata);
                                                                              Ext.ux.ajax.SimManager.init({
                                                                                                              delay: 300,
                                                                                                              defaultSimlet: null
                                                                                                          }).register({'/json/treePanel': {
                                                                                                                              data: JSONdata,
                                                                                                                              stype: 'json'
                                                                                                                          }
                                                                                                                      });
                                                                              debugger;
                                                                              Ext.data.StoreManager.lookup('treeStore').load();
                                                                              Ext.getCmp('treeStore').load();
                                                                              //AndonPortal.store.Tree.load();
                                                                              //treeStore.load();
                                                                              //console.log('AndonPortal.store.Tree', AndonPortal.store.Tree);
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

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
                              'Ext.ux.SqlSocket': 'app/ExtJS-WebSocket/SqlSocket.js'
                          }
                      });
Ext.require (['Ext.ux.WebSocket', 'Ext.ux.WebSocketManager','Ext.ux.SqlSocket']);
//Ext.require (['AndonPortal.store.Tree']); //'AndonPortal.model.Tree',
//var treeStore = Ext.create('AndonPortal.store.Tree',
//                           {   extend: 'Ext.data.Model',
//                               id: 'treeStore',
//                               storeId: 'treeStore',
//                               requires: ['AndonPortal.model.Tree'],
//                               model: Ext.create('AndonPortal.model.Tree'),
//                               pageSize :0,
//                               proxy: {
//                                   type: 'ajax',
//                                   url: '/json/treePanel',
//                                   reader: {
//                                                   type: 'json'
//                                   }
//                               },
//                               autoLoad: false,
//               constructor : function() {
//                   this.callParent(arguments);
//               }
//});

Ext.define('AndonPortal.Application', {
               extend: 'Ext.app.Application',
//               requires: [
//                            'AndonPortal.store.Tree',
//                            'Ext.window.Toast',
//                            'AndonPortal.model.Tree'
//               ],
               name: 'AndonPortal',
//               stores: ['Tree'],
//               models: ['Tree'],
               views: [
                   'DatePicker',
                   'Grid',
                   'Tree',
                   'Viewport'
               ],
               launch: function () {
                   console.log('Application launch function');
//                   treeStore2 = Ext.create('AndonPortal.store.Tree',{
//                                                      id: 'treeStore2',
//                                                      storeId: 'treeStore2'
//                                                      });

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
                                                                              //debugger;
                                                                              //Ext.data.StoreManager.lookup('Tree').load();
                                                                              //Ext.data.StoreManager.lookup('treeStore').load();
                                                                              //Ext.data.StoreManager.lookup('treeStore2').load();
                                                                              //Ext.data.StoreManager.lookup('treeStore3').load();
                                                                              //Ext.getStore('Tree').load();
                                                                              Ext.getStore('treeStore').load();
                                                                              //Ext.getStore('treeStore2').load();
                                                                              //Ext.getStore('treeStore3').load();
                                                                              //treeStore2.load();
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

/**
 * The main application class. An instance of this class is created by app.js when it
 * calls Ext.application(). This is the ideal place to handle application launch and
 * initialization details.
 */
//Ext.Loader.setConfig ({
//                          enabled: true,
//                          paths: {

//                              'Ext.ux.WebSocket': 'app/ExtJS-WebSocket/WebSocket.js' ,
//                              'Ext.ux.WebSocketManager': 'app/ExtJS-WebSocket/WebSocketManager.js'/*,
//                              'Ext.ux.SqlSocket': 'app/ExtJS-WebSocket/SqlSocket.js'*/
//                          }
//                      });
//Ext.require (['Ext.ux.WebSocket', 'Ext.ux.WebSocketManager','Ext.ux.SqlSocket']);
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
               alias: 'andonportal',
//               requires:
//               ['Ext.ux.WebSocket', 'Ext.ux.WebSocketManager','Ext.ux.SqlSocket'],
//               [
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
               controllers: [
                   'Main'
               ],
               launch: function () {
//                   console.log('Application launch function');
                   var qws = Ext.create ('Ext.ux.SqlSocket',{
                                            id:'mysqlsocket',
                                            //url: "ws://127.0.0.1:12346",
                                            url:"ws://10.208.110.75:12346",
                                            listeners: {
                                                ready: function (ws) {
                                                    //debugger;
                                                    Ext.GlobalEvents.fireEvent('appSqlSocketReady',ws);
                                                    AndonPortal.app.fireEvent('appSqlSocketReady',ws);
                                                    console.log('SqlSocket opened');
                                                    this.sql("TREE_GET_JSONTREE",
                                                             "PLANT",
                                                             "JSON_BRANCH",
                                                                     function(resp){
                                                                         var JSONdata=resp.map(function(node) {
                                                                                          return eval('['+node.JSON_BRANCH+']')[0];
                                                                                          });
                                                                         Ext.ux.ajax.SimManager.init({
                                                                                                         delay: 300,
                                                                                                         defaultSimlet: null
                                                                                                     }).register({'/json/treePanel': {
                                                                                                                         data: JSONdata,
                                                                                                                         stype: 'json'
                                                                                                                     }
                                                                                                                 });
                                                                         Ext.getStore('treeStore').load();
                                                                     });
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

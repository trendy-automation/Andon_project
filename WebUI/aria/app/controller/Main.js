Ext.define('AndonPortal.controller.Main', {
    extend: 'Ext.app.Controller',
    alias: 'controller.Main',
    refs: [
        {
            ref: 'treePanel',
            selector: 'mysimpletree'
        },
        {
            ref: 'simplegrid',
            selector: 'grid'
        }
    ],
    exetuteSqlQuery: function(proc,input,fields,result) {
        Ext.GlobalEvents.fireEvent('exetuteSqlQuery',proc,input,fields,result);
    },
    onTreePanelItemClick: function(view, record, item, index, event, options){
        console.log('onTreePanelItemClick',record);
//        this.exetuteSqlQuery('DB_PROCEDURES','',
//                             ['PROCEDURE_NAME', 'INPUT_FIELDS', 'RETURN_FIELDS'],
//                             function(procs){console.log('ws.db.procedures.length',procs.length);});
    },
    onGridItemClick:  function( grid , record , item , index , e , eOpts ) {
        console.log('onGridClick control event',record);
//        this.exetuteSqlQuery('DB_PROCEDURES','',
//                             ['PROCEDURE_NAME', 'INPUT_FIELDS', 'RETURN_FIELDS'],
//                             function(procs){console.log('ws.db.procedures.length',procs.length);});
    },
    onAppSqlSocketReady:  function(ws) {
        //console.log('App sqlsocket ready control event',ws);
        this.exetuteSqlQuery("TREE_GET_JSONTREE",
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
    },
    init: function(application) {
        this.control({
            'treePanel': {
                itemclick: this.onTreePanelItemClick
            },
             'simplegrid' :{
                 itemclick: this.onGridItemClick
             }
        });
    },
               listen : {
                       //listen to events using GlobalEvents
                       global : {
                           appSqlSocketReady : 'onAppSqlSocketReady'
                       }
                   }
});

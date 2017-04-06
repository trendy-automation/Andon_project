Ext.define('AndonPortal.controller.Main', {
    extend: 'Ext.app.Controller',
    alias: 'controller.Main',
//    requires: [
//        'Ext.ux.SqlSocket'
//    ],

//    stores: [
//        'Tree'
//    ],

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

//    renderDynamicMenu: function(view, options) {

//        var dynamicMenus = [];

//        view.body.mask('Loading Menus... Please wait...');

//        this.getMenuStore().load(function(records, op, success){

//            Ext.each(records, function(root){

//                var menu = Ext.create('Packt.view.menu.Tree',{
//                    title: translations[root.get('text')],
//                    iconCls: root.get('iconCls')
//                });

//                var treeNodeStore = root.items(),
//                    nodes = [],
//                    item;

//                for (var i=0; i<treeNodeStore.getCount(); i++){
//                    item = treeNodeStore.getAt(i);

//                   nodes.push({
//                        text: translations[item.get('text')],
//                        leaf: true,
//                        glyph: item.get('iconCls'),
//                        id: item.get('id'),
//                        className: item.get('className')
//                    });
//                }

//                menu.getRootNode().appendChild(nodes);

//                dynamicMenus.push(menu);
//            });

//            view.add(dynamicMenus);
//            view.body.unmask();
//        });
//    },

    onTreePanelItemClick: function(view, record, item, index, event, options){
        /*var mainPanel = this.getMainPanel();

        var newTab = mainPanel.items.findBy(
            function (tab){
                return tab.title === record.get('text');
            });

        if (!newTab){
            newTab = mainPanel.add({
                xtype: record.get('className'),
                glyph: record.get('glyph') + '@FontAwesome',
                title: record.get('text'),
                closable: true
            });
        }

        mainPanel.setActiveTab(newTab);*/
        console.log('onTreePanelItemClick',record);
        //this.redirectTo(record.get('className'));
    },
    onAppSqlSocketReady:  function(ws) {
        console.log('App sqlsocket ready control event',ws);
        ws.sql("TREE_GET_JSONTREE",
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
        Ext.GlobalEvents.fireEvent('sql');
    },
    onGridItemClick:  function( grid , record , item , index , e , eOpts ) {
        console.log('onGridClick control event',record);
    },
    init: function(application) {
        //debugger; //Ext.ComponentQuery.query()
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

Ext.define('AndonPortal.controller.Main', {
    extend: 'Ext.app.Controller',

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
            ref: 'sqlSocket',
            selector: 'mysqlsocket'
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

    onSqlSocketOpen:  function(ws) {
        console.log('sqlsocket open',ws);
    },
    init: function(application) {

        this.control({
            'tree': {
                itemclick: this.onTreePanelItemClick
            },
            'sqlSocket':{
                open: this.onSqlSocketOpen
            }
        });
    }
});

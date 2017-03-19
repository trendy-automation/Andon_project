Ext.define('MyTree', {
                   extend: 'Ext.data.Model',
                   fields: [
                       {name: 'OBJECT_NAME',  type: 'string'},
                       {name: 'OBJECT_TYPE_NAME',  type: 'string'},
                       {name: 'OBJECT_TYPE_ID',   type: 'int', convert: null},
                       {name: 'PKEY',   type: 'int', convert: null},
                       {name: 'PID',   type: 'int', convert: null}
                   ]
           });

var treeStore = Ext.create('Ext.data.TreeStore', {
                               //fields: ['OBJECT_NAME','OBJECT_TYPE_NAME','OBJECT_TYPE_ID','PKEY','PID'],
                               pageSize :0,
                               proxy: {
                                   type: 'ajax',
                                   url: 'trees',
                                   reader: {
                                                   type: 'json',
                                                   rootProperty: 'children'
                                   }
                               },
                               defaultValue: {root:{}},
                               autoLoad: false
                 });

Ext.define('AndonPortal.view.Tree', {
    extend: 'Ext.tree.Panel',
    alias:  'widget.mysimpletree',
//    viewConfig: {plugins: {ptype: 'treeviewdragdrop'}},
    renderTo: Ext.getBody(),
    title: 'TreeGrid',
//    rootVisible: true,
    hideCollapseTool: false,
    collapsible: true,
    width: 300,
    height: 150,
    fields: ['name', 'description'],
    store: treeStore,
    columns: [{
        xtype: 'treecolumn',
        text: 'Andon data',
        dataIndex: "text",//'OBJECT_NAME',
        width: 150,
        sortable: true
    }/*, {
        text: 'Description',
        dataIndex: 'description',
        flex: 1,
        sortable: true
    }*/]
});

Ext.define('TreeObjectNode', {
                   extend: 'Ext.data.Model',
                   fields: [
                       //{name: 'expanded', type: 'boolean', defaultValue: true, persist: false },
                       {name: 'OBJECT_NAME',  type: 'string'},
                       {name: 'OBJECT_TYPE_NAME',  type: 'string'},
                       {name: 'OBJECT_TYPE_ID',   type: 'int', convert: null},
                       {name: 'PKEY',   type: 'int', convert: null},
                       {name: 'PID',   type: 'int', convert: null}
                   ]
           });

var treeStore = Ext.create('Ext.data.TreeStore',
                           {
                               model:'TreeObjectNode',
                               pageSize :0,
                               proxy: {
                                   type: 'ajax',
                                   url: '/json/treePanel',
                                   reader: {
                                                   type: 'json'
                                   }
                               }
                               ,autoLoad: false
                               //,autoSync: true
//                               ,listeners: {
//                                              load: function (treeStore , records , successful , operation , node , eOpts ) {
//                                                  //Ext.getCmp('treePanel').expandPath('/root/');
//                                                   //treePanel.expandPath('/root/');
//                                                  //console.log('treeStore load',records);
//                                              }
//                                          }
                               //,autoLoad: true,
                               //,autoSync: true
                               //clearOnLoad: true,
                 });

Ext.define('AndonPortal.view.Tree', {
    extend: 'Ext.tree.Panel',
    alias:  'widget.mysimpletree',
    id: 'treePanel',
//    viewConfig: {plugins: {ptype: 'treeviewdragdrop'}},
    renderTo: Ext.getBody(),
    title: 'TreeGrid',
    rootVisible: false,
    hideCollapseTool: false,
    collapsible: true,
    width: 300,
    height: 150,
    fields: ['name', 'description'],
    store: treeStore,
    //autoLoad : true,
    columns: [{
        xtype: 'treecolumn',
        text: 'Andon data',
        dataIndex: 'OBJECT_NAME',
        width: 300,
        sortable: true
    }/* ,
    {
        text: 'Description',
        dataIndex: 'description',
        flex: 1,
        sortable: true
    }*/]


    ,listeners: {
               load: function (treeStore , records , successful , operation , node , eOpts ) {
                    node.expand();
               }
           }/*,
    selection : Ext.create(
                    'Ext.selection.Model',{
                    mode:'MULTI'})*/
});

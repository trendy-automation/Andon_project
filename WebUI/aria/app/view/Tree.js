//var treeStore = Ext.create('Ext.data.TreeStore',
//                           {
//                               requires: ['AndonPortal.model.Tree'],
//                               model:'AndonPortal.model.Tree',
//                               pageSize :0,
//                               proxy: {
//                                   type: 'ajax',
//                                   url: '/json/treePanel',
//                                   reader: {
//                                                   type: 'json'
//                                   }
//                               }
//                               ,autoLoad: false
//                 });

//Ext.require (['Ext.selection.Model']);
//Ext.require (['AndonPortal.model.Tree']);
//Ext.require (['AndonPortal.store.Tree']);
Ext.define('AndonPortal.view.Tree', {
    extend: 'Ext.tree.Panel',
    //requires: ['AndonPortal.store.Tree'],
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
    store: AndonPortal.store.Tree,
    //Ext.data.StoreManager.lookup('treeStore'), //'treeStore',//'Tree', //AndonPortal.store.Tree,
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
               load: function (Tree , records , successful , operation , node , eOpts ) {
                    node.expand();
               }
           }/*,
    selection : Ext.create(
                    'Ext.selection.Model',{
                    mode:'MULTI'})*/
});

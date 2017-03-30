Ext.define('AndonPortal.store.Tree',
                           {   extend: 'Ext.data.TreeStore',
                               alias:  'store.mytreestore',
                               requires: ['AndonPortal.model.Tree'],
                               model: Ext.create('AndonPortal.model.Tree'),
                               model: 'AndonPortal.model.Tree',
                               pageSize :0,
                               proxy: {
                                   type: 'ajax',
                                   url: '/json/treePanel',
                                   reader: {
                                                   type: 'json'
                                   }
                               },
//                               storeId: 'treeStore',
                               autoLoad: false,
                               constructor : function() {
                                   this.callParent(arguments);
                               }
});

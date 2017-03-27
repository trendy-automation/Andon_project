Ext.create('AndonPortal.store.Tree',
                           {   extend: 'Ext.data.Model',
                               requires: ['AndonPortal.model.Tree'],
                               model:'AndonPortal.model.Tree',
                               pageSize :0,
                               proxy: {
                                   type: 'ajax',
                                   url: '/json/treePanel',
                                   reader: {
                                                   type: 'json'
                                   }
                               },
                               autoLoad: false,
               alias       : 'store.Tree',
               constructor : function() {
                   this.callParent(arguments);
               },
               storeId: 'treeStore',
               id: 'treeStore'
});

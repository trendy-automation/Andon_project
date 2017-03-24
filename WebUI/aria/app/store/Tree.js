Ext.define('AndonPortal.store.Tree',
                           {   extend: 'Ext.data.Model',
                               model:'AndonPortal.model.Tree',
                               pageSize :0,
                               proxy: {
                                   type: 'ajax',
                                   url: '/json/treePanel',
                                   reader: {
                                                   type: 'json'
                                   }
                               },
                               autoLoad: false
});

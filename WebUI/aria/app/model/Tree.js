Ext.define('AndonPortal.model.Tree', {
                   extend: 'Ext.data.Model',
                   fields: [
                   //{name: 'expanded', type: 'boolean', defaultValue: true, persist: false },
                       {name: 'iconCls', type: 'string', defaultValue: '', convert:function(val,row){
                           var iconCls='';
                           switch (row.get('OBJECT_TYPE_NAME')) {
                           case 'AREA':
                               iconCls='box';
                               break
                           case 'UAP':
                               iconCls='folder';
                               break
                           case 'LINE':
                               iconCls='help';
                               break
                           case 'LINE_INJECTION':
                               iconCls='help';
                               break
                           case 'MACHINE':
                               iconCls='tags';
                               break
                           case 'UNIT':
                               iconCls='unpin';
                               break
                           case 'MOLD':
                               iconCls='stop';
                               break
                           case 'MACHINE_INJECTION':
                               iconCls='inbox';
                               break
                           case 'MACHINE_US_WELD':
                               iconCls='lightning';
                               break
                           case 'LINE_US_WELD':
                               iconCls='help';
                               break
                           case 'MACHINE_PY':
                               iconCls='eye';
                               break
                           case 'ROBOT_INJECTION':
                               iconCls='file';
                               break
                           }
                            return iconCls;}},
                       {name: 'OBJECT_NAME',  type: 'string'},
                       {name: 'OBJECT_TYPE_NAME',  type: 'string'},
                       {name: 'OBJECT_TYPE_ID',   type: 'int', convert: null},
                       {name: 'PKEY',   type: 'int', convert: null},
                       {name: 'PID',   type: 'int', convert: null}
                   ]
});
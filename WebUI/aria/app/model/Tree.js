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
                               iconCls='Простой';
                               break
                           case 'LINE_INJECTION':
                               iconCls='Ошибка';
                               break
                           case 'MACHINE':
                               iconCls='Ошибка';
                               break
                           case 'UNIT':
                               iconCls='Ошибка';
                               break
                           case 'MOLD':
                               iconCls='Ошибка';
                               break
                           case 'MACHINE_INJECTION':
                               iconCls='Ошибка';
                               break
                           case 'MACHINE_US_WELD':
                               iconCls='Ошибка';
                               break
                           case 'LINE_US_WELD':
                               iconCls='Ошибка';
                               break
                           case 'MACHINE_PY':
                               iconCls='Ошибка';
                               break
                           case 'ROBOT_INJECTION':
                               iconCls='Ошибка';
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

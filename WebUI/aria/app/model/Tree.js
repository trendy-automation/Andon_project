Ext.define('AndonPortal.model.Tree', {
                   extend: 'Ext.data.Model',
                   fields: [
                   //{name: 'expanded', type: 'boolean', defaultValue: true, persist: false },
                        //https://icons8.com
                       {name: 'iconCls', type: 'string', persist: false, convert:function(val,row){
                           var iconCls='';
                           switch (row.get('OBJECT_TYPE_NAME')) {
                           case 'AREA':
                               iconCls='solar_panel';
                               break
                           case 'UAP':
                               iconCls='network';
                               break
                           case 'LINE':
                               iconCls='recycling';
                               break
                           case 'LINE_INJECTION':
                               iconCls='recycling';
                               break
                           case 'MACHINE':
                               iconCls='engineering';
                               break
                           case 'UNIT':
                               iconCls='transistor';
                               break
                           case 'MOLD':
                               iconCls='cardboard_box';
                               break
                           case 'MACHINE_INJECTION':
                               iconCls='deployment';
                               break
                           case 'MACHINE_US_WELD':
                               iconCls='robot';
                               break
                           case 'LINE_US_WELD':
                               iconCls='recycling';
                               break
                           case 'MACHINE_PY':
                               iconCls='detective';
                               break
                           case 'ROBOT_INJECTION':
                               iconCls='robot';
                               break
                           default:
                               iconCls='capacitor';
                           }
                           //console.log('AndonPortal.model.Tree iconCls convert');
                            return iconCls;}},
                       {name: 'OBJECT_NAME',  type: 'string'},
                       {name: 'OBJECT_TYPE_NAME',  type: 'string'},
                       {name: 'OBJECT_TYPE_ID',   type: 'int', convert: null},
                       {name: 'PKEY',   type: 'int', convert: null},
                       {name: 'PID',   type: 'int', convert: null}
                   ]/*,
               constructor : function() {
                   console.log('AndonPortal.model.Tree constructor');
                   this.callParent(arguments);
               }*/
});

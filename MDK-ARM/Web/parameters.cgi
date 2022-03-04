t <head>
t     <script language=JavaScript type="text/JavaScript" src="action.js"></script>
t     <link rel="stylesheet" href="style.css">
t </head>
t 
t <body>
t <form action=parameters.cgi method=post name=cgi>
t <div>
t <table class="parameter_table">
t     <tr class="head">
t         <th class="item_param">Item</th>
t         <th class="data_param">Value</th>
t     </tr>
t     <tr>
t         <td>Front And Rear Impact Protection Enable</td>
t         <td>
c p e <input type=hidden name=impact_e value="off" %s><input type=checkbox name=impact_e value="on" %s>
t         </td>
t     </tr>
t     <tr>
t         <td>Front And Rear Impact Protection Distance (mm)</td>
t         <td>
c p d <input type=text name=impact_d value="%d" size=4 maxlength=4>
t         </td>
t     </tr>
t     <tr>
t         <td>Side Impact Protection Enable</td>
t         <td>
c p g <input type=hidden name=impact_g value="off" %s><input type=checkbox name=impact_g value="on" %s>
t         </td>
t     </tr>
t     <tr>
t         <td>Side Impact Protection Distance (mm)</td>
t         <td>
c p f <input type=text name=impact_f value="%d" size=4 maxlength=4>
t         </td>
t     </tr>
t     <tr>
t         <td>Threshold time of impact signal (ms)</td>
t         <td>
c p t <input type=text name=impact_t value="%d" size=4 maxlength=4>
t         </td>
t     </tr>
t </table>
t
t <table class="parameter_table">
t     <tr class="head">
t         <th class="sensor_id">Sensor ID</th>
t         <th class="uart_channel">UART channel</th>
t         <th class="sensor_id">Sensor ID</th>
t         <th class="uart_channel">UART channel</th>
t     </tr>
t     <tr>
t         <td>Sensor 0</td>
t         <td>
c p 0 <input type=text name=sensor_0 value="%d" size=1 maxlength=2>
t         </td>
t         <td>Sensor 1</td>
t         <td>
c p 1 <input type=text name=sensor_1 value="%d" size=1 maxlength=2>
t         </td>
t     </tr>
t     <tr>
t         <td>Sensor 2</td>
t         <td>
c p 2 <input type=text name=sensor_2 value="%d" size=1 maxlength=2>
t         </td>
t         <td>Sensor 3</td>
t         <td>
c p 3 <input type=text name=sensor_3 value="%d" size=1 maxlength=2>
t         </td>
t     </tr>
t     <tr>
t         <td>Sensor 4</td>
t         <td>
c p 4 <input type=text name=sensor_4 value="%d" size=1 maxlength=2>
t         </td>
t         <td>Sensor 5</td>
t         <td>
c p 5 <input type=text name=sensor_5 value="%d" size=1 maxlength=2>
t         </td>
t     </tr>
t     <tr>
t         <td>Sensor 6</td>
t         <td>
c p 6 <input type=text name=sensor_6 value="%d" size=1 maxlength=2>
t         </td>
t         <td>Sensor 7</td>
t         <td>
c p 7 <input type=text name=sensor_7 value="%d" size=1 maxlength=2>
t         </td>
t     </tr>
t </table>
t </div>
t <div class="param_button">
t   <input class="param_confirm_button" type=button name=set value="OK" onclick="changeConfirm(this.form)">
t   <input class="param_reset_button" type=reset value="Reset">
t </div>
t </form>
t </body>
t </html>
.
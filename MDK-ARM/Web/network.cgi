t <head>
t     <script language=JavaScript type="text/JavaScript" src="action.js"></script>
t     <link rel="stylesheet" href="style.css">
t </head>
t 
t <body>
t     <form action=network.cgi method=post name=cgi>
#t         <input type=hidden value="net" name=pg>
t         <table class="net_table">
t                 <tr class="head">
t                     <th class="item">Item</th>
t                     <th class="data">Data</th>
t                 </tr>
t                 <tr>
t                     <td>DHCP Enable</td>
t                     <td>
c n d <input type=hidden name=dhcp value="off" %s><input type=checkbox name=dhcp value="on" %s>
t                     </td>
t                 </tr>
t                 <tr>
t                     <td>IP Address</td>
t                     <td>
c n i <input type=text name=ipad value="%s" size=18 maxlength=18>
t                     </td>
t                 </tr>
t                 <tr>
t                     <td>Subnet Mask</td>
t                     <td>
c n m <input type=text name=mask value="%s" size=18 maxlength=18>
t                     </td>
t                 </tr>
t                 <tr>
t                     <td>UDP Port</td>
t                     <td>
c n p <input type=text name=port value="%d" size=5 maxlength=5>
t              </td>
t          </tr>
t  </table>
t     <div class="param_button">
t       <input class="param_confirm_button" type=button name=set value="OK" onclick="changeConfirm(this.form)">
t       <input class="param_reset_button" type=reset value="Reset">
t     </div>
t   </form>
t </body>
t </html>
.
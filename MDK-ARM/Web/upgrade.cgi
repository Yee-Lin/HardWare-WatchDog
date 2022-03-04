t <form action="upgrade.cgi" method="POST" enctype="multipart/form-data" name=fileupload>
t  <table>
t     <tr>
t     <td><input name="File" type="file" size="50" maxlength="256" class="file_button"></td>
t     </tr>
t   <tr>
t     <td><input type=button value="Upload" OnClick="this.form.submit()" class="upload_button"></td>
c u l <td><p>%s</p></td>
t   </tr>
t  </table>
t </form>
t <form action="upgrade_progress.htm" method="POST" enctype="application/x-www-form-urlencoded" name="upgrade">
c u g <input type="submit" value="Upgrade" style="visibility: %s;" name="upgrade" onclick="this.form.submit()">
t </form>
.
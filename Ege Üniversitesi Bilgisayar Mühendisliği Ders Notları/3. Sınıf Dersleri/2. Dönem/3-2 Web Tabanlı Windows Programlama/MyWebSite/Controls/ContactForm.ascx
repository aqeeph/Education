<%@ Control Language="C#" AutoEventWireup="true" CodeFile="ContactForm.ascx.cs" Inherits="Controls_ContactForm" %>
<style type="text/css">
    .style1
    {
        width: 100%;
    }
    .style2
    {
    }
    .style3
    {
        width: 570px;
    }
</style>

<script type="text/javascript">
    function ValidatePhoneNumbers(source, args)
    {
        var phoneHome = document.getElementById('<%= phoneHomeTextBox.ClientID %>');
        var phoneBusiness = document.getElementById('<%= phoneBusinessTextBox.ClientID %>');
        if (phoneHome.value != '' || phoneBusiness.value != '')
        {
            args.IsValid = true;
        }
        else
        {
            args.IsValid = false;
        }
    }
</script>

<asp:UpdatePanel ID="UpdatePanel1" runat="server">
 <ContentTemplate>

  <table class="style1" runat="server" id="FormTable">
    <tr>
        <td colspan="3">
            Visitors can use this form to get in touch with me.</td>
    </tr>
    <tr>
        <td class="style2">
            Name</td>
        <td class="style3">
            <asp:TextBox ID="nameTextBox" runat="server"></asp:TextBox>
        </td>
        <td>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator1" runat="server" 
                ControlToValidate="nameTextBox" CssClass="ErrorMessage" 
                ErrorMessage="Enter your name">* Enter your name</asp:RequiredFieldValidator>
        </td>
    </tr>
    <tr>
        <td class="style2">
            Email Address</td>
        <td class="style3">
            <asp:TextBox ID="emailAddressTextBox" runat="server"></asp:TextBox>
        </td>
        <td>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator2" runat="server" 
                ControlToValidate="emailAddressTextBox" CssClass="ErrorMessage" 
                Display="Dynamic" ErrorMessage="Enter an e-mail address">*</asp:RequiredFieldValidator>
            <asp:RegularExpressionValidator ID="RegularExpressionValidator1" runat="server" 
                ControlToValidate="emailAddressTextBox" CssClass="ErrorMessage" 
                Display="Dynamic" ErrorMessage="Enter a valid e-mail address" 
                ValidationExpression="\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*">*</asp:RegularExpressionValidator>
        </td>
    </tr>
    <tr>
        <td class="style2">
            Email Address (Repeat)</td>
        <td class="style3">
            <asp:TextBox ID="confirmEmailAddressTextBox" runat="server"></asp:TextBox>
        </td>
        <td>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator3" runat="server" 
                ControlToValidate="confirmEmailAddressTextBox" CssClass="ErrorMessage" 
                Display="Dynamic" ErrorMessage="Confirm the e-mail address">*</asp:RequiredFieldValidator>
            <asp:CompareValidator ID="CompareValidator1" runat="server" 
                ControlToCompare="emailAddressTextBox" 
                ControlToValidate="confirmEmailAddressTextBox" CssClass="ErrorMessage" 
                Display="Dynamic" ErrorMessage="Retype the e-mail address">*</asp:CompareValidator>
        </td>
    </tr>
    <tr>
        <td class="style2">
            Home Phone</td>
        <td class="style3">
            <asp:TextBox ID="phoneHomeTextBox" runat="server"></asp:TextBox>
        </td>
        <td>
            <asp:CustomValidator ID="CustomValidator1" runat="server" 
                ClientValidationFunction="ValidatePhoneNumbers" CssClass="ErrorMessage" 
                Display="Dynamic" ErrorMessage="Enter your home or business phone number" 
                onservervalidate="CustomValidator1_ServerValidate">*</asp:CustomValidator>
        </td>
    </tr>
    <tr>
        <td class="style2">
            Business Phone</td>
        <td class="style3">
            <asp:TextBox ID="phoneBusinessTextBox" runat="server"></asp:TextBox>
        </td>
        <td>
            &nbsp;</td>
    </tr>
    <tr>
        <td class="style2">
            Comments</td>
        <td class="style3">
            <asp:TextBox ID="commentsTextBox" runat="server" Height="163px" 
                TextMode="MultiLine" Width="538px"></asp:TextBox>
        </td>
        <td>
            <asp:RequiredFieldValidator ID="RequiredFieldValidator4" runat="server" 
                ControlToValidate="commentsTextBox" CssClass="ErrorMessage" Display="Dynamic" 
                ErrorMessage="Enter a comment">*</asp:RequiredFieldValidator>
        </td>
    </tr>
    <tr>
        <td class="style2">
            &nbsp;</td>
        <td class="style3">
            <asp:Button ID="sendButton" runat="server" Text="Send" 
                onclick="sendButton_Click" />
        </td>
        <td>
            &nbsp;</td>
    </tr>
    <tr>
        <td class="style2" colspan="3">
            <asp:ValidationSummary ID="ValidationSummary1" runat="server" 
                CssClass="ErrorMessage" 
                
                HeaderText="Please correct the following errors before you press the Send Button:" />
        </td>
    </tr>
</table>

 <asp:Label ID="Message" runat="server" Text="Message Sent!" Visible="False"></asp:Label>

 </ContentTemplate>
</asp:UpdatePanel>

<asp:UpdateProgress ID="UpdateProgress1" runat="server" AssociatedUpdatePanelID="UpdatePanel1">
    <ProgressTemplate>
        <div class="PleaseWait">
            Please Wait...
        </div>
    </ProgressTemplate>
</asp:UpdateProgress>



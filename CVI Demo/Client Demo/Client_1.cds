<distribution version="12.0.1" name="Client" type="MSI64">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{9A985C3D-C95E-4A5D-87FD-B19C58DA055C}" x64="true">
		<general appName="Client" outputLocation="c:\Users\hp\Desktop\Client Demo\cvidistkit.Client_1" relOutputLocation="cvidistkit.Client_1" outputLocationWithVars="c:\Users\hp\Desktop\Client Demo\cvidistkit.%name_1" relOutputLocationWithVars="cvidistkit.%name_1" upgradeBehavior="1" autoIncrement="true" version="1.0.1">
			<arp company="%company" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" readMe="" license="">
			<dlgstrings welcomeTitle="Client" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="[Program Files]" dirID="24" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="Client" dirID="100" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="Client" dirID="101" parentID="24" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\Users\hp\Desktop\Client Demo\cvibuild.client\Release64\client.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="c:\Users\hp\Desktop\Client Demo\client.uir" relSourcePath="client.uir" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput64 targetType="0" dirID="101" projectID="0">
				<fileID>0</fileID></projectOutput64>
			<projectDependencies64 dirID="101" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="Client" targetFileID="0" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
		<mergemodules/>
		<products/>
		<runtimeEngine cvirte="true" instrsup="true" analysis="true" netvarsup="true" activeXsup="true" rtutilsup="true" installToAppDir="false"/><sxsRuntimeEngine>
			<selected>false</selected>
			<doNotAutoSelect>false</doNotAutoSelect></sxsRuntimeEngine>
		<advanced mediaSize="650">
			<launchConditions>
				<condition>MINOS_WINXP_SP0</condition>
			</launchConditions>
			<includeConfigProducts>true</includeConfigProducts>
			<maxImportVisible>silent</maxImportVisible>
			<maxImportMode>merge</maxImportMode>
			<custMsgFlag>false</custMsgFlag>
			<custMsgPath>d:\national instruments\cvi2012\bin\msgrte.txt</custMsgPath>
			<signExe>false</signExe>
			<certificate></certificate>
			<signTimeURL></signTimeURL>
			<signDescURL></signDescURL></advanced>
		<Projects NumProjects="1">
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\Users\hp\Desktop\Client Demo\client.prj" ProjectRelativePath="client.prj"/></Projects>
		<buildData progressBarRate="0.696362662443478">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.055046499999997</ProductsAdded>
				<DPConfigured>1.029811999999993</DPConfigured>
				<DPMergeModulesAdded>3.294814999999999</DPMergeModulesAdded>
				<DPClosed>11.769812999999999</DPClosed>
				<DistributionsCopied>15.469966499999991</DistributionsCopied>
				<End>143.603333999999990</End></progressTimes></buildData>
	</msi>
</distribution>

<distribution version="12.0.1" name="Server" type="MSI64">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{13630D57-2E27-4B4A-8ACD-AF07A487669F}" x64="true">
		<general appName="Server" outputLocation="c:\Users\hp\Desktop\Server Demo\cvidistkit.Server_1" relOutputLocation="cvidistkit.Server_1" outputLocationWithVars="c:\Users\hp\Desktop\Server Demo\cvidistkit.%name_1" relOutputLocationWithVars="cvidistkit.%name_1" upgradeBehavior="1" autoIncrement="true" version="1.0.1">
			<arp company="%company" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" readMe="" license="">
			<dlgstrings welcomeTitle="Server" welcomeText=""/></userinterface>
		<dirs appDirID="101">
			<installDir name="Server" dirID="100" parentID="7" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="Server" dirID="101" parentID="24" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="24" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\Users\hp\Desktop\Server Demo\cvibuild.server\Release64\server.exe" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="c:\Users\hp\Desktop\Server Demo\server.uir" relSourcePath="server.uir" relSourceBase="0" targetDir="101" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput64 targetType="0" dirID="101" projectID="0">
				<fileID>0</fileID></projectOutput64>
			<projectDependencies64 dirID="101" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="Server" targetFileID="0" destDirID="100" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
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
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\Users\hp\Desktop\Server Demo\server.prj" ProjectRelativePath="server.prj"/></Projects>
		<buildData progressBarRate="0.649757604352030">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.054682500000007</ProductsAdded>
				<DPConfigured>0.979683500000007</DPConfigured>
				<DPMergeModulesAdded>3.169683500000005</DPMergeModulesAdded>
				<DPClosed>11.619685500000003</DPClosed>
				<DistributionsCopied>15.319887999999999</DistributionsCopied>
				<End>153.903547000000000</End></progressTimes></buildData>
	</msi>
</distribution>

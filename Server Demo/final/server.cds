<distribution version="12.0.1" name="server" type="MSI64">
	<prebuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></prebuild>
	<postbuild>
		<workingdir>workspacedir</workingdir>
		<actions></actions></postbuild>
	<msi GUID="{03B00177-0683-4D12-82AC-710B0C418AA9}" x64="true">
		<general appName="server" outputLocation="c:\Users\hp\Desktop\Server Demo\final\cvidistkit.server" relOutputLocation="cvidistkit.server" outputLocationWithVars="c:\Users\hp\Desktop\Server Demo\final\cvidistkit.%name" relOutputLocationWithVars="cvidistkit.%name" upgradeBehavior="1" autoIncrement="true" version="1.0.1">
			<arp company="%company" companyURL="" supportURL="" contact="" phone="" comments=""/>
			<summary title="" subject="" keyWords="" comments="" author=""/></general>
		<userinterface language="Chinese (Simplified)" showPaths="true" readMe="" license="">
			<dlgstrings welcomeTitle="server" welcomeText=""/></userinterface>
		<dirs appDirID="100">
			<installDir name="[Start&gt;&gt;Programs]" dirID="7" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="[Program Files]" dirID="24" parentID="-1" isMSIDir="true" visible="true" unlock="false"/>
			<installDir name="server" dirID="100" parentID="24" isMSIDir="false" visible="true" unlock="false"/>
			<installDir name="server" dirID="101" parentID="7" isMSIDir="false" visible="true" unlock="false"/></dirs>
		<files>
			<simpleFile fileID="0" sourcePath="c:\Users\hp\Desktop\Server Demo\final\cvibuild.server\Release64\server.exe" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="1" sourcePath="c:\Users\hp\Desktop\Server Demo\final\server.uir" relSourcePath="server.uir" relSourceBase="0" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/>
			<simpleFile fileID="2" sourcePath="C:\Users\hp\Desktop\Server Demo\final\监控日志.xlsx" targetDir="100" readonly="false" hidden="false" system="false" regActiveX="false" runAfterInstallStyle="IMMEDIATELY_RESUME_INSTALL" cmdLineArgs="" runAfterInstall="false" uninstCmdLnArgs="" runUninst="false"/></files>
		<fileGroups>
			<projectOutput64 targetType="0" dirID="100" projectID="0">
				<fileID>0</fileID></projectOutput64>
			<projectDependencies64 dirID="100" projectID="0"/></fileGroups>
		<shortcuts>
			<shortcut name="Server" targetFileID="0" destDirID="101" cmdLineArgs="" description="" runStyle="NORMAL"/></shortcuts>
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
			<Project000 ProjectID="0" ProjectAbsolutePath="c:\Users\hp\Desktop\Server Demo\final\server.prj" ProjectRelativePath="server.prj"/></Projects>
		<buildData progressBarRate="0.700504862260290">
			<progressTimes>
				<Begin>0.000000000000000</Begin>
				<ProductsAdded>0.080578999999994</ProductsAdded>
				<DPConfigured>0.980577999999994</DPConfigured>
				<DPMergeModulesAdded>3.320580000000006</DPMergeModulesAdded>
				<DPClosed>11.820580000000005</DPClosed>
				<DistributionsCopied>15.520803999999989</DistributionsCopied>
				<End>142.754183999999950</End></progressTimes></buildData>
	</msi>
</distribution>

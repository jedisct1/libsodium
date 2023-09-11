<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="4.0"
  xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Label="Globals">
    <ProjectGuid>{A185B162-6CB6-4502-B03F-B56F7699A8D9}</ProjectGuid>
    <ProjectName>libsodium</ProjectName>
    <PlatformToolset>{{platform}}</PlatformToolset>
  </PropertyGroup>
  <ItemGroup Label="ProjectConfigurations">
    {{configurations}}
  </ItemGroup>
  <PropertyGroup Label="Configuration">
    <ConfigurationType Condition="$(Configuration.IndexOf('DLL')) == -1">StaticLibrary</ConfigurationType>
    <ConfigurationType Condition="$(Configuration.IndexOf('DLL')) != -1">DynamicLibrary</ConfigurationType>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="PropertySheets">
    <Import Project="$(ProjectDir)..\..\properties\$(Configuration).props" />
    <Import Project="$(ProjectDir)..\..\properties\Output.props" />
    <Import Project="$(ProjectDir)$(ProjectName).props" />
  </ImportGroup>
  <ItemGroup>
    {{v1}}
  </ItemGroup>
  <ItemGroup>
    {{v2}}
    <ClInclude Include="..\..\resource.h" />
  </ItemGroup>
  <ItemGroup>
    <ResourceCompile Include="..\..\resource.rc">
    </ResourceCompile>
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets">
  </ImportGroup>
</Project>

# Betaverse

Unreal Engine Version: 5.0

This project is a proof of concept on how to build a very simple metaverse with Unreal Engine and Epic Online Services. The Project uses C++ and blueprints.

## Launching the Project
The Betaverse Project is a C++ project and therefore has to be compiled from source first.
To generate the visual studio files right click the Betaverse.uproject file and select "Generate Visual Studio project files"
https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/DevelopmentSetup/VisualStudioSetup/


## Ready Player Me
To use the implemented ReadyPlayerMe character creator
add a new folder in the project's root folder called "Plugins".
Download the SDK from:
https://docs.readyplayer.me/ready-player-me/integration-guides/unreal-engine-4
Paste the glTFRuntime folder and the ReadyPlayerMe folder into the created Plugins folder.

## EOS
To Host and Join Sessions EOS have to be enabled.
To Setup EOS go to https://dev.epicgames.com/portal and register a new product.
Your product information then has to be entered in the engine under:

Project Settings->Plugins->OnlineSubsystemEOS->Add Artifacts fill in information from registered eos product

EOS Documentation:
https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Online/EOS/








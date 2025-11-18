OVERVIEW 
=================
Ambiq's Certificate Generation Tools provide tools for generating certificates using the Signature Plugin that handles the local/remote signing process, isolated from the main scripts, allowing for easy customization of the security aspects of the RSA key access. Also included, is an example remote Hardware Security Module (HSM) for signing data. 
This README provides an overview of the tools, procedures for modifying/creating new plugins, and overview of the configuration files, and details about the simulated HSM script.

Signature Plugin Framework
==========================================
Ambiq's sample plugins for certificate generation are located in oem_tools_pkg/cert_utils/cert_gen_utils/plugin/signature_interface.py. 
As shipped, these plugins can be configured for either local key signing (keys reside on the same machine) or remote signing (keys reside on a remote machine, and accessed via URL).
It is important to note that these sample plugins provided are for reference only and the user is encouraged to create a new plugin to match their specific needs. Future tools updates will maintain the plugin interface and isolate the user’s specific changes from tools updates.

By default, the tools as delivered, are configured to use the 'LocalInterface' plugin is used to generate all example certificates. The configuration can be easily changed to use the example HSM plugin or any newly created plugins (details below).
New plugins should be implemented within the cert_gen_utils/plugin directory following the guidelines outlined below.

Guidelines For Creating New Plugins
===========================================
To create a new plugin, follow these procedures:

1. Inheritance: Plugin classes must inherit from the 'Interface' abstract base class.
2. Method Implementation: Plugin classes must implement the sign(keyIdx: str, data: bytes) method, which is responsible for generating signatures for the given data.
3. Configuration File: Plugins can have their own configuration file in the cert_gen_utils/plugin directory
   The plugin configuration file should contain plugin-specific data such as the path to a public key table, signing service URL, username/password, etc. 
   These configurations will be parsed and used to initialize the plugin class.


Using Ambiq's Example Configuration Files
============================================
Ambiq provides example configuration files used during the certificate generation process. These files can be found in the cert_gen_utils/am_config directory. 
When generating certificates using a plugin, ensure that the following fields within the respective certificate config file are updated:

+ auth-key: A unique key identifier that is used to specify the proper certificate key/keypair and/or password from a public key table or a remote HSM. This field is parsed by each 
            of the respective generation scripts and passed to the plugin's sign(keyIdx: str, data: bytes) function. 
            Please ensure that the plugin implementation correctly handles the conversion of the key identifier (`keyIdx`) from string format to the appropriate data type required for key lookup operations.
            The examples, utilize hex indexes, therefore the LocalInterface and HTTPInterface plugins cast to this type.

+ signature-plugin-path: Path combination containing the module's path and plugin class name. The path is relative to the /cert_gen_utils directory.
    + Format: package_name.module_name.module_class_name 
        + package_name: The directory that contains the plugin script.
        + module_name: The name of the plugin script.
        + module_class_name: The name of the implemented class in the plugin script.

+ signature-plugin-cfg: Path to the plugin configuration file which is relative to the /cert_gen_utils directory and should reside in the cert_gen_utils/plugin directory.

+ auth-pubkey: Path to the respective public key used for the certificate generation process.

Sample Configuration File Entries
===========================================
Here are sample entries for the signature-plugin-path and signature-plugin-cfg fields in the configuration file:

If your plugin script is within the /plugin directory and named my_plugin.py and the class within it is MyPluginClass, the signature-plugin-path would be:
signature-plugin-path = plugin.my_plugin.MyPluginClass

If your plugin configuration file is named my_plugin_config.ini then the signature-plugin-cfg would be:
signature-plugin-cfg = ./plugin/my_plugin_config.ini


Remote Signing Example 
===========================================

Ambiq also provides a simulated hsm script for signing data using RSA encryption. 
This script can be located in cert_gen_utils/plugin_reference/sign_server.py.

- Dependencies
    Flask - pip install flask
    Requests - pip install requests
    Ensure urllib3 version < 2.0 (Tested with python3 -m pip install urllib3==1.26.6)

- Initialization
    Before running the script, ensure that the necessary dependencies are installed. 
    You can use a virtual environment to manage dependencies and isolate the environment.

    1)
         Open a terminal and run following command to start the simulated hsm:

         /oem_tools_pkg/oem_asset_prov_utils/oem_key_request/plugin_references$ python3 sign_server.py
    
         This will open a local dev server on your devices local host.

    2)
        For each certificate that is intended to be generated, two fields within their respective certificate configuration file need to be changed.
        These two fields are the signature-plugin-path and signature-plugin-cfg field.
        
        These two fields are defaulted to:
        ---------------------------------
        signature-plugin-path = plugin.signature_interface.LocalInterface
        signature-plugin-cfg = ./plugin/am_local_keys_plugin.cfg

        Which should be changed to:
        ---------------------------------
        signature-plugin-path = plugin.sign_server_interface.HTTPInterface
        signature-plugin-cfg = ./plugin/am_remote_keys_plugin.cfg

        Please ensure that this changed is made to each certificate configuration file that is intended to be used to generate it's respective certificate.

    3)
        Next, remote plugin data need to be entered within the plugin/am_remote_keys_plugin.cfg file. This example uses local host so the url should remain as http://127.0.0.1:5000.
        user-id, and password are two fields that are shown purely to show other possilbe fields that one might need in their implementation, these fields are implemented in this exmaple. 

    4) 
        In a separate terminal run the python commands to generate each of the respective certificates.
        You should notice the simulated HSM terminal is being updated with the endpoint data given to it.

- Key Management 
    The sign_server.py script loads key information from a key table file specified by KEY_PATH. 
    This key table contains information about private keys, including keypair files and key password files.
    If one intends to use another key table, the KEY_PATH variable will need to be updated accordingly

- Signing Data:
    The /sign/<int:keyIdx> endpoint is used to sign data. It takes a key index as a parameter and expects data to be signed as a query parameter (data). 
    The data is signed using RSA encryption, and the resulting signature is base64 encoded for transmission.

- Verification:
    The /verify/<int:key_index> endpoint is intended for key verification, and simply checks if the given key index matches a key file within the key table.
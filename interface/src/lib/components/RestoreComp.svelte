<script lang="ts">
    import { onMount } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "$lib/components/toasts/notifications";
    import Button from './Button.svelte';
    import Save from '~icons/tabler/device-floppy';
    import Upload from '~icons/tabler/file-upload';
    import Download from '~icons/tabler/file-download';
    import type { WifiSettings, ApSettings } from '$lib/types/models';


    interface SequencerState {
        config: {
            commands: unknown[];
            [key: string]: unknown;
        };
        [key: string]: unknown;
    }

    interface SequencerConfig {
        hostname: string;
        commands: unknown[];
        apChannel?: number;
        espnowChannel?: number;
    }

    interface EspNowSettings {
        enabled: boolean;
        channel: number;
        retryDelay: number;
        autoPing: boolean;
        messageHistory: string[];
    }

    let hostname: string = "";
    let config: SequencerConfig = {
        hostname: "",
        commands: []
    };
    let wifiSettings: WifiSettings = {} as WifiSettings;
    let sequencerState: SequencerState = {} as SequencerState;

    async function downloadConfig() {
        try {
            // Get hostname from WiFi settings 
            wifiSettings = await getJsonRest('/rest/wifiSettings', {} as WifiSettings);
            if (wifiSettings) {
                hostname = wifiSettings.hostname || "";
            }
            
            // Get sequencer commands
            sequencerState = await getJsonRest('/rest/spin-seq/sequencer', {} as SequencerState);
            
            // Get AP settings for channel
            const apSettings = await getJsonRest('/rest/apSettings', {} as ApSettings);
            
            // Get ESPNow settings for channel
            const espnowSettings = await getJsonRest('/rest/espnow', {} as EspNowSettings);
            
            config = {
                hostname: hostname,
                commands: sequencerState.config.commands,
                apChannel: apSettings?.channel,
                espnowChannel: espnowSettings?.channel
            };
            const dataStr = JSON.stringify(config, null, 2);
            const dataBlob = new Blob([dataStr], { type: 'application/json' });
            const url = URL.createObjectURL(dataBlob);
            
            const link = document.createElement('a');
            link.href = url;
            link.download = `${hostname || 'config'}.json`;
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
            URL.revokeObjectURL(url);

            notifications.success('Configuration downloaded successfully', 3000);
        } catch (err) {
            notifications.error('Failed to load configuration', 3000);
        }
    }

    async function handleFileUpload(event: Event) {
        const input = event.target as HTMLInputElement;
        const file = input.files?.[0];
        
        if (file) {
            try {
                const text = await file.text();
                const importedConfig = JSON.parse(text) as SequencerConfig;
                
                // Validate imported config structure
                if (!importedConfig.commands || !Array.isArray(importedConfig.commands) || !importedConfig.hostname) {
                    throw new Error('Invalid configuration file format');
                }
                else {
                    await postJsonRest('/rest/spin-seq/sequencer', {config:{commands:importedConfig.commands}});
                }

                // Update WiFi settings (hostname)
                wifiSettings = await getJsonRest('/rest/wifiSettings', {} as WifiSettings);
                if (wifiSettings) {
                    wifiSettings.hostname = importedConfig.hostname;
                    await postWiFiSettings(wifiSettings);
                }

                // Update AP channel if provided
                if (typeof importedConfig.apChannel === 'number') {
                    const apSettings = await getJsonRest('/rest/apSettings', {} as ApSettings);
                    console.log(apSettings)
                    if (apSettings) {
                        apSettings.channel = importedConfig.apChannel;
                        await fetch('/rest/apSettings', {
                            method: 'POST',
                            headers: { 'Content-Type': 'application/json' },
                            body: JSON.stringify(apSettings)
                        });
                    }
                }

                // Update ESPNow channel if provided
                if (typeof importedConfig.espnowChannel === 'number') {
                    await postJsonRest('/rest/espnow', {channel: importedConfig.espnowChannel});
                }

                notifications.success('Configuration imported successfully', 3000);
            } catch (err) {
                const error = err as Error;
                notifications.error('Failed to import configuration: ' + (error.message || 'Unknown error'), 3000);
            }
        }
    }

	async function postWiFiSettings(data: WifiSettings) {
		try {
			const response = await fetch('/rest/wifiSettings', {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(data)
			});
			if (response.status == 200) {
				notifications.success('Wi-Fi settings updated.', 3000);
				wifiSettings = await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}
</script>

<SettingsCard>
    <span slot="title">Backup & Restore</span>
    <div class="flex flex-col space-y-4">
        <div class="flex flex-row gap-4">
            <Button label="Export Configuration" onClick={downloadConfig}>
                <Download class="h-5 w-5" />
            </Button>
            <label class="btn btn-primary">
                <Upload class="h-5 w-5 mr-2" />
                Import Configuration
                <input
                    type="file"
                    accept=".json"
                    on:change={handleFileUpload}
                    class="hidden"
                />
            </label>
        </div>
    </div>
</SettingsCard>
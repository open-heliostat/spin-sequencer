<script lang="ts">
    import { getJsonRest } from '$lib/stores/rest';
    import Spinner from './Spinner.svelte';
    import type { SpinDiagnostics, SpinRemote } from '$lib/types/models';
	import SettingsCard from './SettingsCard.svelte';
	import StatusPanel from './StatusPanel.svelte';
	import Collapsible from './Collapsible.svelte';
    import Remote from '~icons/tabler/network';

    export let remote: SpinRemote;
    let diag: SpinDiagnostics = {} as SpinDiagnostics;
    export let onChange: () => void;

    async function getDiag() {
        if (remote.ip || remote.rxId) {
            let path = remote.ip ? "http://" + remote.ip + "/rest/spin-seq/diag" : "/rest/can/tun/" + remote.rxId + "/spin-seq/diag";
            return getJsonRest(path, diag, {signal: AbortSignal.timeout(1000)}).then((data) => {
                diag = data;
                updateRemote();
                return diag;
            }).catch((error) => {
                if (remote.ip && remote.rxId) {
                    return getJsonRest("/rest/can/tun/" + remote.rxId + "/spin-seq/diag", diag, {signal: AbortSignal.timeout(2000)}).then((data) => {
                        diag = data;
                        updateRemote();
                        return diag;
                    }).catch((error) => {
                        console.error("Failed to get diagnostics: ", error);
                    });
                }
            });
        }
    }
    
    async function updateRemote() {
        let changed = false;
        if (diag.wifi.hostname != remote.hostname) {
            remote.hostname = diag.wifi.hostname;
            changed = true;
        }
        if (diag.wifi.ip != remote.ip) {
            remote.ip = diag.wifi.ip;
            changed = true;
        }
        if (diag.can.rxId != remote.rxId) {
            remote.rxId = diag.can.rxId;
            changed = true;
        }
        if (changed) {
            onChange();
        }
    }

</script>

<SettingsCard>
    <Remote slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">{remote.hostname}</span>
    <Collapsible>
        <span slot="title">Diagnostics</span>
        {#await getDiag()}
            <Spinner></Spinner>
        {:then diag}
            <StatusPanel
                diag={diag}
            />
        {/await}
    </Collapsible>
</SettingsCard>
<script lang="ts">
    import { getJsonRest } from '$lib/stores/rest';
    import Spinner from './Spinner.svelte';
    import type { SpinDiagnostics, SpinRemote } from '$lib/types/models';
	import SettingsCard from './SettingsCard.svelte';
	import StatusPanel from './StatusPanel.svelte';
	import Collapsible from './Collapsible.svelte';
    import Remote from '~icons/tabler/network';
	import { onMount, onDestroy } from 'svelte';
	import Slider from './Slider.svelte';
    import Button from './Button.svelte';
	import { sequence } from '@sveltejs/kit/hooks';

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

    async function getSequencerData() {
        if (remote.ip) {
            let path = "http://" + remote.ip + "/rest/spin-seq/diag/sequencer";
            return getJsonRest(path, diag.sequencer, {signal: AbortSignal.timeout(1000)}).then((data) => {
                diag.sequencer = data;
                return diag;
            }).catch((error) => {
                console.error("Failed to get sequencer data: ", error);
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

    let intervalID: any;
    onMount(() => {
        intervalID = setInterval(() => {
            if (diag.sequencer.isRunning) getSequencerData();
        }, 1278);
        getSequencerData();
    });
    onDestroy(() => {
        clearInterval(intervalID);
    });

</script>

<SettingsCard>
    <Remote slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">
        <!-- Open Link in new tab if IP is set -->
        {#if remote.ip}
        <a href="http://{remote.ip}" target="_blank" rel="noopener noreferrer">
            {remote.hostname}
        </a>
        {:else}
        {remote.hostname}
        {/if}
    </span>
    {#if diag?.sequencer}
    <Slider
        label="Sequencer Command"
        min={0}
        max={diag.sequencer.numCommands - 1}
        step={1}
        disabled={diag.sequencer.numCommands == 0}
        hasNumber
        bind:value={diag.sequencer.selectedCommand}
    />
    {/if}
    <Collapsible>
        <span slot="title">Diagnostics</span>
        {#await getDiag()}
            <Spinner></Spinner>
        {:then diag}
            <StatusPanel
                diag={diag}
            />
        {/await}
        {#if remote.ip}
        <Button
            label="Open UI"
            onClick={() => {
                window.open("http://" + remote.ip, "_blank");
            }}
        />
        {/if}
    </Collapsible>
</SettingsCard>
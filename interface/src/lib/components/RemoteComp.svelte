<script lang="ts">
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { getJsonRestWithCanFallback, postJsonRestWithCanFallback, getJsonRestWithHostnameFallback } from '$lib/stores/remote';
    import Spinner from './Spinner.svelte';
    import type { SpinDiagnostics, SpinRemote } from '$lib/types/models';
	import SettingsCard from './SettingsCard.svelte';
	import StatusPanel from './StatusPanel.svelte';
	import Collapsible from './Collapsible.svelte';
    import Remote from '~icons/tabler/network';
    import Firmware from '~icons/tabler/refresh-alert';
	import { onMount, onDestroy } from 'svelte';
	import Slider from './Slider.svelte';
    import Button from './Button.svelte';
    import { openModal, closeAllModals } from 'svelte-modals';
    import ConfirmDialog from './ConfirmDialog.svelte';
    import { notifications } from '$lib/components/toasts/notifications';
    import Cancel from '~icons/tabler/x';
    import CloudDown from '~icons/tabler/cloud-download';
    import GithubUpdateDialog from './GithubUpdateDialog.svelte';
    import { compareVersions } from 'compare-versions';
	import Checkbox from './Checkbox.svelte';
	import { sequence } from '@sveltejs/kit/hooks';

    export let remote: SpinRemote;
    export let diag: SpinDiagnostics = {} as SpinDiagnostics;
    export let onChange: () => void;

    export async function getDiag() {
        return getJsonRestWithHostnameFallback("/spin-seq/diag", diag, remote.ip, remote.hostname).then((data) => {
            diag = data;
            updateRemote();
            return diag;
        }).catch((error) => {
            console.error("Failed to get diagnostics: ", error);
        });
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

    export async function runCommand(command: number, enableSeq = false) {
        let path = "http://" + (remote.ip || remote.hostname) + "/rest/spin-seq/sequencer";
        return postJsonRest(path, {control:{execute:command}})
            .then(() => {if (enableSeq && !diag?.sequencer?.isRunning) {setSequencerState(true); diag.sequencer.isRunning = true;}});
    }

    export async function setSequencerState(state: boolean) {
        let path = "http://" + (remote.ip || remote.hostname) + "/rest/spin-seq/sequencer";
        return postJsonRest(path, {control:{run:state}});
    }

    async function updateRemote() {
        let changed = false;
        if (diag.wifi && diag.wifi.hostname != remote.hostname) {
            remote.hostname = diag.wifi?.hostname;
            changed = true;
        }
        if (diag.wifi && diag.wifi?.ip != remote.ip) {
            remote.ip = diag.wifi?.ip;
            changed = true;
        }
        if (diag.can && diag.can?.rxId != remote.rxId) {
            remote.rxId = diag.can?.rxId;
            changed = true;
        }
        if (diag.espnow && diag.espnow?.macAddress != remote.macAddress) {
            remote.macAddress = diag.espnow.macAddress;
            changed = true;
        }
        if (changed) {
            onChange();
        }
    }

    let intervalID: any;
    onMount(() => {
        intervalID = setInterval(() => {
            if (diag?.sequencer?.isRunning) getSequencerData();
        }, 1278);
        getSequencerData();
        if (!remote.hostname || !remote.ip || !remote.version) {
            getDiag();
        }
    });
    onDestroy(() => {
        clearInterval(intervalID);
    });

</script>

<SettingsCard>
    <Remote slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title" class="h-7">
        <a href={"http://" + (remote.ip ? remote.ip : (remote.hostname + ".local"))} target="_blank" rel="noopener noreferrer">
            {remote.hostname || remote.ip}
        </a>
        <!-- <button
            class="btn btn-square btn-ghost h-7 w-7 ml-2"
            on:click={confirmUpdate}
        >
            <div class="h-7 content-center items-center self-start">
                <span class="indicator-item indicator-top indicator-center badge badge-info badge-xs top-2 scale-75 lg:top-1">
                    {remote.version}
                </span>
                <Firmware class="h-7 w-7" />
            </div>
        </button> -->
    </span>
    {#if diag?.sequencer}
    <Checkbox
        label="Sequencer State"
        bind:value={diag.sequencer.isRunning}
        onChange={() => setSequencerState(diag.sequencer.isRunning)}
    />
    <Slider
        label="Sequencer Command"
        min={0}
        max={diag.sequencer.numCommands - 1}
        step={1}
        disabled={diag.sequencer.numCommands == 0}
        hasNumber
        onChange={() => runCommand(diag.sequencer.selectedCommand)}
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
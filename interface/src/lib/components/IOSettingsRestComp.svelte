<script lang="ts">
    import { onDestroy, onMount } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from './GridForm.svelte';
    import NumberInput from './NumberInput.svelte';
    import Checkbox from './Checkbox.svelte';
    import Button from './Button.svelte';
    import Spinner from './Spinner.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';

    export let restPath: string = '/rest/spin-seq/io';

    type IOConfig = {
        startButtonPin: number;
        startButtonActiveLow: boolean;
        startButtonDebounceMs: number;
        statusLedPin: number;
        statusLedActiveHigh: boolean;
    };

    type IODebugState = {
        startButtonPressed: boolean;
        startButtonReading: boolean;
        statusLedState: boolean;
    };

    let ioConfig: IOConfig;
    let ioDebugState: IODebugState = {
        startButtonPressed: false,
        startButtonReading: false,
        statusLedState: false
    };
    let isDirty = false;
    let isSaving = false;
    let loadPromise: Promise<IOConfig>;
    let refreshTimer: ReturnType<typeof setInterval> | undefined;

    function markDirty() {
        isDirty = true;
    }

    async function loadConfig() {
        try {
            const data = await getJsonRest(restPath, ioConfig as IOConfig & Partial<IODebugState>);
            ioConfig = data;
            ioDebugState.startButtonPressed = Boolean(data.startButtonPressed);
            ioDebugState.startButtonReading = Boolean(data.startButtonReading);
            ioDebugState.statusLedState = Boolean(data.statusLedState);
            isDirty = false;
            return data;
        } catch (error) {
            console.error('Failed to load IO config', error);
            throw error;
        }
    }

    async function refreshDebugState() {
        try {
            const data = await getJsonRest(restPath, {} as Partial<IOConfig & IODebugState>);
            if (typeof data.startButtonPressed === 'boolean') {
                ioDebugState.startButtonPressed = data.startButtonPressed;
            }
            if (typeof data.startButtonReading === 'boolean') {
                ioDebugState.startButtonReading = data.startButtonReading;
            }
            if (typeof data.statusLedState === 'boolean') {
                ioDebugState.statusLedState = data.statusLedState;
            }
        } catch {
        }
    }

    async function saveConfig() {
        isSaving = true;
        try {
            const data = await postJsonRest(restPath, ioConfig);
            ioConfig = data;
            isDirty = false;
            return data;
        } catch (error) {
            console.error('Failed to save IO config', error);
            throw error;
        } finally {
            isSaving = false;
        }
    }

    loadPromise = loadConfig();

    onMount(() => {
        refreshDebugState();
        refreshTimer = setInterval(refreshDebugState, 500);
    });

    onDestroy(() => {
        if (refreshTimer) {
            clearInterval(refreshTimer);
        }
    });
</script>

<SettingsCard isDirty={isDirty}>
    {#snippet title()}
        <span>I/O Pins</span>
    {/snippet}

    {#await loadPromise}
        <Spinner />
    {:then}
        <GridForm>
            <NumberInput
                label="Start Button Pin"
                bind:value={ioConfig.startButtonPin}
                min={0}
                max={48}
                step={1}
                onChange={markDirty}
            />
            <Checkbox
                label="Start Button Active Low"
                bind:value={ioConfig.startButtonActiveLow}
                onChange={markDirty}
            />
            <NumberInput
                label="Start Button Debounce (ms)"
                bind:value={ioConfig.startButtonDebounceMs}
                min={0}
                max={5000}
                step={5}
                onChange={markDirty}
            />
            <NumberInput
                label="Status LED Pin"
                bind:value={ioConfig.statusLedPin}
                min={0}
                max={48}
                step={1}
                onChange={markDirty}
            />
            <Checkbox
                label="Status LED Active High"
                bind:value={ioConfig.statusLedActiveHigh}
                onChange={markDirty}
            />
        </GridForm>
        <div class="mt-2 flex flex-wrap items-center gap-2 text-sm">
            <span class="opacity-70">Start Button</span>
            {#if ioConfig.startButtonPin < 0}
                <span class="badge badge-ghost">Disabled</span>
            {:else}
                <span class={`badge ${ioDebugState.startButtonPressed ? 'badge-success' : 'badge-ghost'}`}>
                    {ioDebugState.startButtonPressed ? 'Pressed' : 'Released'}
                </span>
                <span class={`badge ${ioDebugState.startButtonReading ? 'badge-info' : 'badge-ghost'}`}>
                    Raw {ioDebugState.startButtonReading ? 'HIGH' : 'LOW'}
                </span>
            {/if}
            <span class={`badge ${ioDebugState.statusLedState ? 'badge-success' : 'badge-ghost'}`}>
                LED {ioDebugState.statusLedState ? 'ON' : 'OFF'}
            </span>
        </div>
        <div class="flex justify-end gap-2">
            <Button
                label="Save"
                onClick={saveConfig}
                disabled={!isDirty || isSaving}
            />
        </div>
    {/await}
</SettingsCard>

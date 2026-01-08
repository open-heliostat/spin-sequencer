<script lang="ts">
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

    let ioConfig: IOConfig;
    let isDirty = false;
    let isSaving = false;
    let loadPromise: Promise<IOConfig>;

    function markDirty() {
        isDirty = true;
    }

    async function loadConfig() {
        try {
            const data = await getJsonRest(restPath, ioConfig as IOConfig);
            ioConfig = data;
            isDirty = false;
            return data;
        } catch (error) {
            console.error('Failed to load IO config', error);
            throw error;
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
        <div class="flex justify-end gap-2">
            <Button
                label="Save"
                onClick={saveConfig}
                disabled={!isDirty || isSaving}
            />
        </div>
    {/await}
</SettingsCard>

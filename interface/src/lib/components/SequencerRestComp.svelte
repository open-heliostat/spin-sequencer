<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import Spinner from '$lib/components/Spinner.svelte';
    import GridForm from '$lib/components/GridForm.svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import DisableButton from './DisableButton.svelte';
    import StopButton from './StopButton.svelte';
	import Text from './Text.svelte';
	import Slider from './Slider.svelte';

    export let restPath: string;

    interface SequencerState {
        status: {
            isRunning: boolean;
            selectedCommand: number;
            commandRunning: boolean;
            nextCommand: number;
            lostCommands: number;
        };
        config: {
            selectedCommand: number;
            isRunning: boolean;
        };
    }

    let sequencerState: SequencerState;
    let commandInput = '';

    let intervalID: any;
    onMount(() => {
        intervalID = setInterval(() => {
            getSequencerStatus();
        }, 987);
    });
    onDestroy(() => {
        clearInterval(intervalID);
    });

    async function getSequencerStatus() {
        return getJsonRest(restPath + "/status", sequencerState?.status)
            .then((data) => {
                sequencerState.status = data;
            });
    }

    async function getSequencerState() {
        return getJsonRest(restPath, sequencerState).then((data) => sequencerState = data);
    }

    function selectCommand(index: number) {
        return postJsonRest(restPath + '/control', { select: index });
    }

    function toggleRun() {
        return postJsonRest(restPath + '/control', { 
            run: !sequencerState?.status.isRunning 
        });
    }

    function executeCommand() {
        if (commandInput) {
            postJsonRest(restPath + '/control', { 
                execute: commandInput 
            }).then((data) => {
                console.log(data);
                commandInput = '';
            });
        }
    }
</script>

<SettingsCard>
    <span slot="title">Sequencer</span>
    {#await getSequencerState()}
        <Spinner></Spinner>
    {:then nothing}
        <div>
            <GridForm>
                <Slider
                    label="Select"
                    bind:value={sequencerState.config.selectedCommand}
                    min={0} 
                    max={32} 
                    step={1}>
                </Slider>
                <Text
                    label="Command"
                    bind:value={commandInput}
                    onChange={executeCommand}>
                </Text>

            </GridForm>
            <Collapsible> 
                <span slot="title">Status</span>
                <div class="status-grid">
                    <div>Running: 
                        <span class={sequencerState?.status.isRunning ? 'active' : ''}>
                            {sequencerState?.status.isRunning ? 'Yes' : 'No'}
                        </span>
                    </div>
                    <div>Selected: {sequencerState?.status.selectedCommand}</div>
                    <div>Command Running: {sequencerState?.status.commandRunning ? 'Yes' : 'No'}</div>
                    <div>Next: {sequencerState?.status.nextCommand}</div>
                    <div>Lost Commands: {sequencerState?.status.lostCommands}</div>
                </div>
            </Collapsible>
            <div class="flex flex-row flex-wrap justify-between gap-x-2">
                <button class="btn btn-primary" 
                    on:click={toggleRun}
                >{sequencerState?.status.isRunning ? 'Stop' : 'Run'} Sequence</button>
                <div class="flex-grow"></div>
                <div>
                    <DisableButton onClick={() => postJsonRest(restPath + '/control', { run: false })}></DisableButton>
                    <StopButton onClick={() => postJsonRest(restPath + '/control', { run: false })}></StopButton>
                </div>
            </div>
        </div>
    {/await}
</SettingsCard>

<style>
    .status-grid {
        display: grid;
        grid-template-columns: repeat(2, 1fr);
        gap: 0.5rem;
        margin-bottom: 1rem;
    }
    .active {
        color: var(--success);
        font-weight: bold;
    }
</style>
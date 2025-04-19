<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import Spinner from '$lib/components/Spinner.svelte';
    import GridForm from '$lib/components/GridForm.svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "$lib/components/toasts/notifications";
    import StopButton from './StopButton.svelte';
	import Text from './Text.svelte';
	import Slider from './Slider.svelte';
	import Button from './Button.svelte';
	import Checkbox from './Checkbox.svelte';

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
            commands: Object[];
            selectedCommand: number;
            isRunning: boolean;
        };
    }

    let sequencerState: SequencerState;
    // let commandInput = '';
    let commandError = '';
    let commandsJsonString = "";

    $: commandsCount = sequencerState?.config.commands.length - 1 || 0;
    $: commandInput = JSON.stringify(sequencerState?.config.commands[sequencerState?.config.selectedCommand]) || '';

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
                // sequencerState.config.selectedCommand = data.selectedCommand;
                // commandInput = JSON.stringify(sequencerState.config.commands[data.selectedCommand]);
            });
    }

    async function getSequencerState() {
        return getJsonRest(restPath, sequencerState).then((data) => {
            sequencerState = data;
            commandsJsonString = JSON.stringify(sequencerState.config.commands)
                .replaceAll("[{", "[\n    {")
                .replaceAll("},{", "},\n    {")
                .replaceAll("}]", "}\n]");
        });
    }

    function selectCommand(index: number) {
        if (index >= 0) {
            commandInput = JSON.stringify(sequencerState.config.commands[index]);
        }
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
            }).then((data) => { // @ts-ignore
                if (data.execute.error) { // @ts-ignore
                    commandError = data.execute.error;
                    notifications.error(commandError, 3000);
                } else {
                    notifications.success("Command succesfully parsed : " + data.execute, 3000);
                }
                commandInput = '';
            });
        }
    }
    
    function writeCommands() {
        if (commandsJsonString) {
            sequencerState.config.commands = JSON.parse(commandsJsonString);
            console.log(sequencerState.config.commands);
            postJsonRest(restPath + '/config', sequencerState.config)
                .then((data) => {sequencerState.config = data; console.log(data);});
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
                <Checkbox
                    label="Enabled"
                    bind:value={sequencerState.status.isRunning}
                    onChange={() => postJsonRest(restPath + '/control', { run: sequencerState.status.isRunning })}>
                </Checkbox>
                <Slider
                    label="Select"
                    bind:value={sequencerState.config.selectedCommand}
                    min={0} 
                    bind:max={commandsCount} 
                    step={1}
                    onChange={() => selectCommand(sequencerState.config.selectedCommand)}>
                </Slider>
                <Text
                    label="Command"
                    bind:value={commandInput}
                    onChange={executeCommand}>
                </Text>

            </GridForm>
            <div class="flex flex-row flex-wrap justify-between gap-x-2">
                <div class="flex-grow"></div>
                <Button 
                    onClick={executeCommand}
                    label="Run"
                />
                <Button 
                    onClick={writeCommands}
                    label="Write"
                />
            </div>
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

                <textarea
                    class="textarea"
                    bind:value={commandsJsonString}
                    on:change={executeCommand}
                    placeholder="Commands JSON"
                />
            </Collapsible>
            <div class="flex flex-row flex-wrap justify-between gap-x-2">
                <Button 
                    onClick={writeCommands}
                    label="Write Commands"
                />
                <div class="flex-grow"></div>
                <div>
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
    textarea {
        field-sizing: content; /* used to be `form-sizing` but changed to this */
    }
</style>
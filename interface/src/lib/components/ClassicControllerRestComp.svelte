<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
    import Slider from '$lib/components/Slider.svelte';
    import Checkbox from '$lib/components/Checkbox.svelte';
    import GridForm from '$lib/components/GridForm.svelte';
    import Spinner from '$lib/components/Spinner.svelte';
    import SettingsCard from './SettingsCard.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import DisableButton from './DisableButton.svelte';
    import StopButton from './StopButton.svelte';

    export let restPath: string;

    interface ClassicControllerState {
        position: number;
        isMoving: boolean;
        pingPong: boolean;
        stopPosition: number;
        dist: number;
        speed: number;
    }

    let controllerState: ClassicControllerState;

    let intervalID: any;
    onMount(() => {
        intervalID = setInterval(() => {
            getPosition();
        }, 987);
    });
    onDestroy(() => {
        clearInterval(intervalID);
    });

    async function getState() {
        return getJsonRest(restPath + "/state", controllerState)
            .then((data) => controllerState = data);
    }

    async function getPosition() {
        return getJsonRest(restPath + "/state/position", 0.)
            .then((data) => controllerState.position = data);
    }

    function setPosition(position: number) {
        return postJsonRest(restPath + '/control', { 
            position: position 
        });
    }

    function setPingPong(enabled: boolean) {
        return postJsonRest(restPath + '/control', { 
            pingpong: enabled 
        });
    }

    function stop() {
        return postJsonRest(restPath + '/control', { 
            stop: true 
        });
    }

    function updateSettings() {
        return postJsonRest(restPath + '/control', {
            stopPosition: controllerState.stopPosition,
            dist: controllerState.dist,
            speed: controllerState.speed
        });
    }
</script>

<SettingsCard>
    <span slot="title">Classic Controller</span>
    {#await getState()}
        <Spinner />
    {:then nothing}
        <div>
            <GridForm>
                <Slider 
                    label="Set Position" 
                    bind:value={controllerState.position}
                    min={0} 
                    max={360}
                    step={0.01}
                    onChange={() => setPosition(controllerState.position)}
                />
                <Checkbox 
                    label="Ping-Pong" 
                    bind:value={controllerState.pingPong}
                    onChange={() => setPingPong(controllerState.pingPong)}
                />
                <Slider 
                    label="Stop Position" 
                    bind:value={controllerState.stopPosition}
                    min={0} 
                    max={360}
                    step={0.01}
                    onChange={updateSettings}
                />
                <Slider 
                    label="Distance" 
                    bind:value={controllerState.dist}
                    min={0} 
                    max={360}
                    step={0.01}
                    onChange={updateSettings}
                />
                <Slider 
                    label="Speed" 
                    bind:value={controllerState.speed}
                    min={-100} 
                    max={100}
                    step={1}
                    onChange={updateSettings}
                />
            </GridForm>

            <Collapsible>
                <span slot="title">Status</span>
                <GridForm>
                    <div class="status-grid">
                        <div>Position: {controllerState?.position.toFixed(2)}°</div>
                        <div>Status: {controllerState?.isMoving ? 'Moving' : 'Stopped'}</div>
                        <div>Mode: {controllerState?.pingPong ? 'Ping-Pong' : 'Normal'}</div>
                    </div>
                </GridForm>
            </Collapsible>
        </div>
    {/await}
    
    <div class="flex flex-row flex-wrap justify-between gap-x-2">
        <div class="flex-grow" />
        <div>
            <StopButton onClick={() => stop()} />
        </div>
    </div>
</SettingsCard>

<style>
    .status-grid {
        display: grid;
        grid-template-columns: repeat(2, 1fr);
        gap: 0.5rem;
        margin-bottom: 1rem;
    }
</style>
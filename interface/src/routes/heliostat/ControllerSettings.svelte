<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
	import type { ControllerState } from '$lib/types/models'
	import Slider from '$lib/components/Slider.svelte';
	import Checkbox from '$lib/components/Checkbox.svelte';
	import GridForm from '$lib/components/GridForm.svelte';
	import { Chart, registerables } from 'chart.js';
	import { daisyColor } from '$lib/DaisyUiHelper';
	import { cubicOut } from 'svelte/easing';
	import { slide } from 'svelte/transition';
	import { analytics } from '$lib/stores/analytics';

    export let controllerState : ControllerState | undefined;
    export let label: string;
    export let onChange = () => {};
    export let sendEvent = ({}) => {};
    export let hydrate = () => {};
    

	Chart.register(...registerables);

    let offsetsChartElement: HTMLCanvasElement;
    let offsetsChart: Chart;

    onMount(() => {
        offsetsChart = new Chart(offsetsChartElement, {
            type: 'line',
            data: {
				labels: [],
                datasets: [
                    {
                        label: label + ' calibration data',
                        borderColor: daisyColor('--p'),
                        backgroundColor: daisyColor('--p', 50),
                        borderWidth: 2,
                        data: [],
                        yAxisID: 'y'
                    }
                ]
            },
            options: {
                maintainAspectRatio: false,
                responsive: true,
                elements: {
                    point: {
                        radius: 1
                    }
                },
                scales: {
                    x: {
                        grid: {
                            color: daisyColor('--bc', 10)
                        },
                        ticks: {
                            color: daisyColor('--bc')
                        },
                        display: false
                    },
                    y: {
                        type: 'linear',
                        title: {
                            display: true,
                            text: 'Offset',
                            color: daisyColor('--bc'),
                            font: {
                                size: 16,
                                weight: 'bold'
                            }
                        },
                        position: 'left',
                        grid: { color: daisyColor('--bc', 10) },
                        ticks: {
                            color: daisyColor('--bc')
                        },
                        border: { color: daisyColor('--bc', 10) }
                    }
                }
            }
        });
        setInterval(() => {
            updateData();
        }, 2000);
    });

    function updateData() {
        if (controllerState?.calibration?.offsets) {
            let offsets = controllerState.calibration.offsets;
            offsetsChart.data.labels = offsets;
            offsetsChart.data.datasets[0].data = offsets;
            offsetsChart.scales['y'].min = Math.min(...offsets);
            offsetsChart.scales['y'].max = Math.max(...offsets);
            offsetsChart.update('none');
        }
    }

</script>

<Collapsible on:opened={hydrate} open={true} class="shadow-lg">
	<span slot="title">{label} Controller</span>
    {#if controllerState}
        {#if controllerState.limits}
            <div>
                <span class="text-lg">Limits</span>
                <GridForm>
                    <Checkbox 
                        label="Enable" 
                        bind:value={controllerState.limits.enabled}
                    ></Checkbox>
                    {#if controllerState.limits.enabled}
                    <Slider 
                        label="Begin" 
                        bind:value={controllerState.limits.begin}
                        min={0} 
                        max={360} 
                        step={0.01}
                        {onChange}
                    ></Slider>
                    <Slider 
                        label="End" 
                        bind:value={controllerState.limits.end}
                        min={0} 
                        max={360} 
                        step={0.01}
                        {onChange}
                    ></Slider>
                    {/if}
                </GridForm>
            </div>
        {/if}
        {#if controllerState.calibration}
            <div>
                <span class="text-lg">Calibration</span>
                <GridForm>
                    <Checkbox 
                        label="Enable" 
                        bind:value={controllerState.calibration.enabled}
                    ></Checkbox>
                    {#if controllerState.calibration.enabled}
                    {/if}
                    {#if controllerState.calibration.offsets}
                    {/if}
                </GridForm>
                <div class="flex flex-row flex-wrap justify-between gap-x-2">
                    <div class="flex-grow"></div>
                    <div>
                        <button class="btn btn-primary inline-flex items-center" on:click={() => {sendEvent({calibration:{start:{}}});}}
                            ><span>Start</span></button
                        >
                        <button class="btn btn-primary inline-flex items-center" on:click={() => {sendEvent({calibration:{stop:{}}});}}
                            ><span>Stop</span></button
                        >
                        <button class="btn btn-primary inline-flex items-center" on:click={() => {sendEvent({calibration:{offsets:{}}});}}
                            ><span>Get Data</span></button
                        >
                        <button class="btn btn-primary inline-flex items-center" on:click={() => {sendEvent({limits:{enabled:true}});}}
                            ><span>Save</span></button
                        >
                    </div>
                </div>
            </div>
        {/if}
    {/if}

</Collapsible>

<div class="w-full overflow-x-auto">
    <div
        class="flex w-full flex-col space-y-1 h-52"
        transition:slide|local={{ duration: 300, easing: cubicOut }}
    >
        <canvas bind:this={offsetsChartElement} />
    </div>
</div>